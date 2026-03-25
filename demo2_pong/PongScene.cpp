// Created by Patricio Palma on 20-03-26.

#include "PongScene.h"
#include "AIController.h"
#include "FontManager.h"
#include "GameEngine.h"
#include "PlayerController.h"
#include "PongState.h"
#include "SceneOrchestrator.h"
#include "ShapeComponent.h"
#include "TextComponent.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <string>

// ---------------------------------------------------------------------------
// PaddleController — human player (W/S). Y-axis moves the paddle.
// ---------------------------------------------------------------------------
class PaddleController : public PlayerController {
public:
    explicit PaddleController(float screenH)
        : PlayerController(0), screenH_(screenH) {
        bind("MoveUp",   SDL_SCANCODE_UP);
        bind("MoveDown", SDL_SCANCODE_DOWN);
    }

protected:
    void update(const float dt) override {
        if (!hasPawn()) return;

        constexpr float speed  = 400.f;
        constexpr float halfH  = 50.f;   // half of paddle height (100px)

        auto& pos = getPawn()->transform.position;
        pos.y += getMovementAxis().y * speed * dt;
        pos.y  = std::clamp(pos.y, halfH, screenH_ - halfH);
    }

private:
    float screenH_;
};

// ---------------------------------------------------------------------------
// PaddleAI — tracks the ball's Y position at limited speed.
// ---------------------------------------------------------------------------
class PaddleAI : public AIController {
public:
    explicit PaddleAI(float screenH) : screenH_(screenH) {}

protected:
    void update(float dt) override {
        if (!hasPawn()) return;

        if (!ball_) ball_ = getScene()->findObject("Ball");
        if (!ball_) return;

        constexpr float speed = 270.f;
        constexpr float halfH = 50.f;

        auto& pos = getPawn()->transform.position;
        const float diff = ball_->transform.position.y - pos.y;
        const float step = std::min(std::abs(diff), speed * dt);
        pos.y += (diff >= 0.f ? step : -step);
        pos.y  = std::clamp(pos.y, halfH, screenH_ - halfH);
    }

private:
    float   screenH_;
    Object* ball_{nullptr};
};

// ---------------------------------------------------------------------------
// BallController — moves the ball, handles wall/paddle bounces and scoring.
// ---------------------------------------------------------------------------
class BallController : public AIController {
public:
    BallController(float screenW, float screenH)
        : screenW_(screenW), screenH_(screenH) {}

protected:
    void onPossess(Object*) override {
        p1Paddle_ = getScene()->findObject("P1Paddle");
        p2Paddle_ = getScene()->findObject("P2Paddle");

        if (p1Paddle_)
            p1PaddlePrevY_ = p1Paddle_->transform.position.y;

        if (auto* obj = getScene()->findObject("P1Score"))
            scoreText1_ = obj->getComponent<TextComponent>("Text");
        if (auto* obj = getScene()->findObject("P2Score"))
            scoreText2_ = obj->getComponent<TextComponent>("Text");
    }

    void update(float dt) override {
        if (!hasPawn()) return;

        // --- Track P1 paddle velocity for hit influence ---
        if (p1Paddle_ && dt > 0.f) {
            p1PaddleVelY_  = (p1Paddle_->transform.position.y - p1PaddlePrevY_) / dt;
            p1PaddlePrevY_ =  p1Paddle_->transform.position.y;
        }

        // --- Launch delay ---
        if (!launched_) {
            launchTimer_ -= dt;
            if (launchTimer_ <= 0.f) launch();
            return;
        }

        auto& pos = getPawn()->transform.position;
        pos.x += dx_ * kSpeed * dt;
        pos.y += dy_ * kSpeed * dt;

        // --- Top / bottom wall bounce ---
        if (pos.y - kRadius <= 0.f) {
            pos.y = kRadius;
            dy_   = std::abs(dy_);
        } else if (pos.y + kRadius >= screenH_) {
            pos.y = screenH_ - kRadius;
            dy_   = -std::abs(dy_);
        }

        // --- Paddle collision ---
        if (p1Paddle_) handlePaddleCollision(p1Paddle_);
        if (p2Paddle_) handlePaddleCollision(p2Paddle_);

        // --- Scoring ---
        if (pos.x + kRadius < 0.f) {
            ++PongState::scoreP2;
            onScore();
        } else if (pos.x - kRadius > screenW_) {
            ++PongState::scoreP1;
            onScore();
        }
    }

private:
    // -----------------------------------------------------------------------
    void launch() {
        // Random angle in ±30° from horizontal, random left/right direction.
        const float angle =
            static_cast<float>((rand() % 60) - 30) * std::numbers::pi_v<float> / 180.f;
        const float signX = (rand() % 2 == 0) ? 1.f : -1.f;
        dx_ = signX * std::cos(angle);
        dy_ = std::sin(angle);
        launched_ = true;
    }

    // -----------------------------------------------------------------------
    // AABB-circle paddle check. Deflection angle depends on hit position.
    void handlePaddleCollision(Object* paddle) {
        const bool isLeft  = paddle->transform.position.x < screenW_ * 0.5f;

        // Only test when the ball is approaching the paddle.
        if ( isLeft && dx_ >= 0.f) return;
        if (!isLeft && dx_ <= 0.f) return;

        const Vec2F& padPos = paddle->transform.position;
        const float left   = padPos.x - kPaddleHW;
        const float right  = padPos.x + kPaddleHW;
        const float top    = padPos.y - kPaddleHH;
        const float bottom = padPos.y + kPaddleHH;

        auto& ballPos = getPawn()->transform.position;
        const bool overlap =
            ballPos.x - kRadius < right  &&
            ballPos.x + kRadius > left   &&
            ballPos.y - kRadius < bottom &&
            ballPos.y + kRadius > top;

        if (!overlap) return;

        // Compute exit angle based on where on the paddle the ball hit.
        // Center → horizontal, edge → ±60°.
        const float hitNorm  = std::clamp((ballPos.y - padPos.y) / kPaddleHH, -1.f, 1.f);
        const float exitAngle = hitNorm * (std::numbers::pi_v<float> / 3.f);  // ±60°

        dx_ = isLeft ? std::cos(exitAngle) : -std::cos(exitAngle);
        dy_ = std::sin(exitAngle);

        // When P1 (human) hits, transfer a fraction of paddle velocity to the
        // ball's Y direction so the player can influence the trajectory.
        // P2 (AI) uses the standard angle-only deflection.
        if (isLeft) {
            const float velNorm = std::clamp(p1PaddleVelY_ / kPaddleMaxSpeed, -1.f, 1.f);
            dy_ += velNorm * kVelTransfer;
            // Renormalize to keep ball speed constant.
            const float len = std::hypot(dx_, dy_);
            if (len > 0.f) { dx_ /= len; dy_ /= len; }
        }

        // Push ball outside the paddle to prevent sticking.
        ballPos.x = isLeft ? right + kRadius + 1.f : left - kRadius - 1.f;
    }

    // -----------------------------------------------------------------------
    void onScore() {
        refreshScoreText();

        if (PongState::scoreP1 >= PongState::winScore) {
            PongState::winner = 1;
            SceneOrchestrator::instance().goTo("gameover");
            return;
        }
        if (PongState::scoreP2 >= PongState::winScore) {
            PongState::winner = 2;
            SceneOrchestrator::instance().goTo("gameover");
            return;
        }

        resetBall();
    }

    void resetBall() {
        getPawn()->transform.position = {screenW_ * 0.5f, screenH_ * 0.5f};
        launched_    = false;
        launchTimer_ = 1.f;
    }

    void refreshScoreText() {
        if (scoreText1_) scoreText1_->setText(std::to_string(PongState::scoreP1));
        if (scoreText2_) scoreText2_->setText(std::to_string(PongState::scoreP2));
    }

    // -----------------------------------------------------------------------
    static constexpr float kSpeed        = 350.f;
    static constexpr float kRadius       =   8.f;
    static constexpr float kPaddleHW     =   7.5f;
    static constexpr float kPaddleHH     =  50.f;
    static constexpr float kPaddleMaxSpeed = 400.f;  // must match PaddleController speed
    static constexpr float kVelTransfer  =   0.4f;   // fraction of paddle velocity added to dy

    float  screenW_, screenH_;
    float  dx_{1.f}, dy_{0.f};
    float  launchTimer_{1.f};
    bool   launched_{false};

    float  p1PaddlePrevY_{0.f};   // paddle Y at end of previous frame
    float  p1PaddleVelY_{0.f};    // estimated paddle Y velocity this frame

    Object*        p1Paddle_{nullptr};
    Object*        p2Paddle_{nullptr};
    TextComponent* scoreText1_{nullptr};
    TextComponent* scoreText2_{nullptr};
};

// ===========================================================================
// PongScene::load
// ===========================================================================
void PongScene::load() {
    PongState::reset();
    backgroundColor = {0x1a, 0x1a, 0x2e, 0xff};

    auto& [sw, sh] = getScreenSize();
    // const float fW = static_cast<float>(sw);
    // const float fH = static_cast<float>(sh);

    TTF_Font* fontScore = FontManager::instance().load("assets/Roboto-Regular.ttf", 48.f);
    TTF_Font* fontHud   = FontManager::instance().load("assets/Roboto-Regular.ttf", 16.f);

    const Vec2F     paddleSize{15.f, 100.f};
    const float     ballRadius = 8.f;
    const SDL_Color white{255, 255, 255, 255};
    const SDL_Color gray {100, 100, 100, 255};

    // --- Dashed center divider ---
    auto* centerLine = addObject<Object>("CenterLine");
    centerLine->transform.position = {sw * 0.5f, 0.f};
    for (int i = 0; i < 30; ++i) {
        auto* dash = centerLine->addComponent<SquareComponent>(
            "Dash" + std::to_string(i), Vec2F{2.f, 10.f}, gray);
        dash->setOffset({0.f, 10.f + i * 20.f});
    }

    // --- Paddles ---
    auto* p1 = addObject<Object>("P1Paddle");
    p1->transform.position = {40.f, sh * 0.5f};
    p1->addComponent<SquareComponent>("Shape", paddleSize, white);

    auto* p2 = addObject<Object>("P2Paddle");
    p2->transform.position = {sw - 40.f, sh * 0.5f};
    p2->addComponent<SquareComponent>("Shape", paddleSize, white);

    // --- Ball ---
    auto* ball = addObject<Object>("Ball");
    ball->transform.position = {sw * 0.5f, sh * 0.5f};
    ball->addComponent<CircleComponent>("Shape", ballRadius, white);

    // --- Score display ---
    auto* p1ScoreObj = addObject<Object>("P1Score");
    p1ScoreObj->transform.position = {sw * 0.25f, 50.f};
    p1ScoreObj->addComponent<TextComponent>("Text", "0", fontScore);

    auto* p2ScoreObj = addObject<Object>("P2Score");
    p2ScoreObj->transform.position = {sw * 0.75f, 50.f};
    p2ScoreObj->addComponent<TextComponent>("Text", "0", fontScore);

    // --- HUD hint ---
    auto* hud = addObject<Object>("HUD");
    hud->transform.position = {sw * 0.5f, sh - 20.f};
    hud->addComponent<TextComponent>("Hint", "Up/Down | First to 10 wins", fontHud);

    // --- Controllers ---
    addPlayerController<PaddleController>(sh)->possess(p1);
    addPlayerController<PaddleAI>(sh)->possess(p2);
    addPlayerController<BallController>(sw, sh)->possess(ball);
}
