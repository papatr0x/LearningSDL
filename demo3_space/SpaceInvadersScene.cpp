// Created by Patricio Palma on 20-03-26.

#include "SpaceInvadersScene.h"
#include "FontManager.h"
#include "PlayerController.h"
#include "SceneOrchestrator.h"
#include "ScriptComponent.h"
#include "ShapeComponent.h"
#include "TextComponent.h"
#include <algorithm>
#include <format>

constexpr float        kFireRate        = 0.25f;
constexpr float        kBulletSpeed     = 600.f;
constexpr Vec2F        kBulletSize      {2.f, 10.f};
constexpr float        kSpeed           = 400.f;
constexpr float        kHalfW           = 25.f;
constexpr float        kMargin          = 10.f;
constexpr SDL_Color    kMainColor       {0x00, 0xff, 0x00, 0xff};
constexpr SDL_Color    kBackgroundColor {0x1a, 0x1a, 0x2e, 0xff};
constexpr Vec2F        kPlayerSize      {40.f, 15.f};

// ---------------------------------------------------------------------------
// ShipController
// ---------------------------------------------------------------------------
class ShipController : public PlayerController {
public:
    ShipController(float screenWidth, TextComponent* scoreText)
        : PlayerController(0), screenWidth_(screenWidth), scoreText_(scoreText) {
        bind("Exit",      SDL_SCANCODE_ESCAPE);
        bind("MoveLeft",  SDL_SCANCODE_LEFT);
        bind("MoveRight", SDL_SCANCODE_RIGHT);
        bind("Fire",      SDL_SCANCODE_SPACE);
    }

protected:
    void update(const float dt) override {
        if (!hasPawn()) return;

        if (isActionPressed("Exit"))
            SceneOrchestrator::instance().goTo("menu");

        auto& pos = getPawn()->transform.position;
        pos.x += getMovementAxis().x * kSpeed * dt;
        pos.x  = std::clamp(pos.x, kHalfW + kMargin, screenWidth_ - kHalfW - kMargin);

        fireCooldown_ -= dt;
        if (isActionDown("Fire") && fireCooldown_ <= 0.f) {
            spawnBullet(pos);
            fireCooldown_ = kFireRate;
            scoreText_->setText(std::format("{:05}", ++score_));
        }
    }

private:
    void spawnBullet(const Vec2F from) const {
        auto* bullet = getScene()->addObject("Bullet");
        bullet->transform.position = {from.x, from.y - 10.f};
        bullet->addComponent<SquareComponent>("Shape", kBulletSize, kMainColor);
        bullet->addComponent<ScriptComponent>("Move", [](const float dt, Object* self) {
            self->transform.position.y -= kBulletSpeed * dt;
            if (self->transform.position.y < -10.f)
                self->destroy();
        });
    }

    float          screenWidth_;
    TextComponent* scoreText_;
    float          fireCooldown_{0.f};
    int            score_{0};
};

// ---------------------------------------------------------------------------
// MartianMoveComponent
// ---------------------------------------------------------------------------
class MartianMoveComponent : public Component {
public:
    MartianMoveComponent(Object* owner, const std::string& id,
                         const std::vector<Object*>& martians, float screenWidth)
        : Component(owner, id), martians_(martians), screenWidth_(screenWidth) {
        setUpdateInterval(kPauseInterval);
    }

    void updateComponent(const float) noexcept override {
        std::erase_if(martians_, [](const Object* m) { return m->isPendingDestroy(); });
        if (martians_.empty()) return;

        if (waiting_) {
            // Pause expired — begin the march
            waiting_ = false;
            setUpdateInterval(kStepInterval);
            return;
        }

        // Clamp cursor after possible removals
        if (cursor_ >= static_cast<int>(martians_.size()))
            cursor_ = 0;

        // Move one martian this tick
        martians_[cursor_]->transform.position.x += kStepX * dir_;
        ++cursor_;

        if (cursor_ >= static_cast<int>(martians_.size())) {
            // Full sweep done — check boundary, then enter pause
            cursor_ = 0;

            float minX = martians_[0]->transform.position.x;
            float maxX = martians_[0]->transform.position.x;
            for (const auto* m : martians_) {
                minX = std::min(minX, m->transform.position.x);
                maxX = std::max(maxX, m->transform.position.x);
            }

            if (minX + kStepX * dir_ < kBorder || maxX + kStepX * dir_ > screenWidth_ - kBorder) {
                for (auto* m : martians_)
                    m->transform.position.y += kStepY;
                dir_ = -dir_;
            }

            waiting_ = true;
            setUpdateInterval(kPauseInterval);
        }
    }

private:
    static constexpr float kPauseInterval = 1.5f;
    static constexpr float kStepInterval  = 0.02f;
    static constexpr float kStepX         = 8.f;
    static constexpr float kStepY         = 20.f;
    static constexpr float kBorder        = 20.f;

    std::vector<Object*> martians_;
    float                screenWidth_;
    float                dir_{1.f};
    int                  cursor_{0};
    bool                 waiting_{true};
};

// ---------------------------------------------------------------------------
// SpaceInvadersScene::load
// ---------------------------------------------------------------------------
void SpaceInvadersScene::load() {
    setBackgroundColor(kBackgroundColor);

    TTF_Font* fontScore = FontManager::instance().load("assets/VT323-Regular.ttf", 24.f);

    auto& [sw, sh] = getScreenSize();

    // Player
    auto* player = addObject("Player");
    player->transform.position = {sw * 0.5f, sh - 90.f};
    player->addComponent<SquareComponent>("Shape", kPlayerSize, kMainColor);

    // UI (created before controller so scoreText pointer is valid at possess time)
    constexpr float uiRow1 = 25.f;
    constexpr float uiRow2 = uiRow1 + 28.f;
    auto* ui = addObject("UI");
    ui->addComponent<TextComponent>("TextScore1Hdr", "SCORE<1>", fontScore, kMainColor, Vec2F{sw * 0.25f, uiRow1});
    ui->addComponent<TextComponent>("TextHScoreHdr", "HI-SCORE", fontScore, kMainColor, Vec2F{sw * 0.50f, uiRow1});
    ui->addComponent<TextComponent>("TextScore2Hdr", "SCORE<2>", fontScore, kMainColor, Vec2F{sw * 0.75f, uiRow1});
    playerScoreTextCompo_ = ui->addComponent<TextComponent>("TextScore1", "00000", fontScore, kMainColor, Vec2F{sw * 0.25f, uiRow2});
    ui->addComponent<TextComponent>("TextHiScore",  "00000",   fontScore, kMainColor, Vec2F{sw * 0.50f, uiRow2});
    ui->addComponent<TextComponent>("TextScore2",   "00000",   fontScore, kMainColor, Vec2F{sw * 0.75f, uiRow2});
    ui->addComponent<TextComponent>("TxtCredit", "CREDIT 00", fontScore, kMainColor, Vec2F{sw * 0.85f, 760.f});

    addPlayerController<ShipController>(static_cast<float>(sw), playerScoreTextCompo_)->possess(player);

    // Martians
    constexpr Vec2F     kMartianSize     {33.f, 30.f};
    constexpr int       kEnemiesPerLine  = 11;
    constexpr int       kEnemyLines      = 5;
    constexpr float     kEnemiesStep     = kMartianSize.x * 1.2f;
    constexpr float     kEnemiesStart    = 40.f;
    constexpr SDL_Color kMartianColor    {0xff, 0xff, 0xff, 0xff};

    float base = 300.f;
    for (int i = 0; i < kEnemyLines; ++i) {
        for (int j = 0; j < kEnemiesPerLine; ++j) {
            auto* martian = addObject(std::format("Martian{}-{}", i, j));
            martian->transform.position = {kEnemiesStart + kEnemiesStep * j, base};
            martian->addComponent<SquareComponent>("Shape", kMartianSize, kMartianColor);
            martians_.push_back(martian);
        }
        base -= kMartianSize.y + 10.f;
    }
    addObject("MoveMartians")->addComponent<MartianMoveComponent>("TheMoverCompo", martians_, static_cast<float>(sw));

    // Shields
    constexpr int   kShieldCount  = 4;
    constexpr float kShieldMargin = 110.f;
    for (int i = 0; i < kShieldCount; ++i) {
        auto* shield = addObject(std::format("Shield{}", i));
        shield->transform.position = {kShieldMargin + (sw - 2 * kShieldMargin) * (i / 3.f), 650.f};
        shield->addComponent<SquareComponent>("Shape", Vec2F{70.f, 50.f}, kMainColor);
    }

    // Bottom line
    auto* line = addObject("LineObject");
    line->transform.position = {sw * 0.5f, 750.f};
    line->addComponent<SquareComponent>("Line", Vec2F{static_cast<float>(sw), 1.f}, kMainColor);
}