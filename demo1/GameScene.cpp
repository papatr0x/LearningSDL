// Created by Patricio Palma on 09-03-26.

#include "GameScene.h"
#include "AIController.h"
#include "Animation.h"
#include "AnimatorComponent.h"
#include "AudioComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "FontManager.h"
#include "GameEngine.h"
#include "MusicComponent.h"
#include "PlayerController.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TextureManager.h"
#include "TransitionComponent.h"

// ---------------------------------------------------------------------------
// Player controller — drives the player pawn with pre-mapped bindings.
// P0 defaults (WASD + Space/LCtrl + gamepad) are inherited from PlayerController.
// ---------------------------------------------------------------------------
class MyPlayerController : public PlayerController {
public:
    MyPlayerController() : PlayerController(0) {
        bind("Exit",          SDL_SCANCODE_ESCAPE);
        bind("CameraToggle",  SDL_SCANCODE_P);
    }

protected:
    void update(float dt) override {
        if (!hasPawn()) return;

        constexpr float speed = 100.f;
        getPawn()->transform.position += getMovementAxis() * speed * dt;

        if (getInput().isMouseButtonPressed(SDL_BUTTON_LEFT))
            getPawn()->transform.position =
                getPawn()->getScene()->screenToWorld(getInput().getMousePosition());

        if (isActionPressed("Exit"))
            getPawn()->getComponent<TransitionComponent>()->trigger();

        if (isActionPressed("CameraToggle"))
            camMode_ = (camMode_ + 1) % 3;

        auto& cam = getPawn()->getScene()->getCamera();
        if (camMode_ == 1) {
            cam.position = getPawn()->transform.position;
        } else if (camMode_ == 2) {
            // Lazy lookup — enemy is created after this controller in load().
            if (!enemy_) enemy_ = getPawn()->getScene()->findObject("Enemy");
            if (enemy_)  cam.position = enemy_->transform.position;
        }
    }

private:
    int     camMode_{0};
    Object* enemy_{nullptr};
};

// ---------------------------------------------------------------------------
// Enemy AI — bounces around the screen and reflects off the player on contact.
// Discovers AudioComponent and collider callbacks from the pawn in onPossess.
// ---------------------------------------------------------------------------
class MyEnemyBounceAI : public AIController {
public:
    MyEnemyBounceAI(float screenW, float screenH)
        : screenW_(screenW), screenH_(screenH),
          dirX_((rand() > RAND_MAX / 2) ? 1.f : -1.f),
          dirY_((rand() > RAND_MAX / 2) ? 1.f : -1.f) {}

protected:
    void onPossess(Object* pawn) override {
        bounceSfx_ = pawn->getComponent<AudioComponent>("Bounce SFX");

        if (auto* col = pawn->getComponent<CircleColliderComponent>())
            col->onEnter = [this](ColliderComponent* other) { onHit(other); };
    }

    void update(float dt) override {
        if (!hasPawn()) return;

        constexpr float speed = 100.f;
        Vec2F& pos = getPawn()->transform.position;
        pos.x += dirX_ * speed * dt;
        pos.y += dirY_ * speed * dt;

        bool bounced = false;
        if (pos.x > screenW_ || pos.x < 0.f) { dirX_ = -dirX_; bounced = true; }
        if (pos.y > screenH_ || pos.y < 0.f) { dirY_ = -dirY_; bounced = true; }
        if (bounced && bounceSfx_) bounceSfx_->play();
    }

private:
    void onHit(ColliderComponent* other) {
        if (other->getOwner()->getTag() != "player") return;

        // Reflect velocity: v' = v - 2(v·n)n
        const Vec2F n_raw = getPawn()->transform.position - other->getOwner()->transform.position;
        const float len = n_raw.length();
        if (len == 0.f) return;
        const Vec2F n   = n_raw * (1.f / len);
        const float dot = dirX_ * n.x + dirY_ * n.y;
        dirX_ -= 2.f * dot * n.x;
        dirY_ -= 2.f * dot * n.y;

        if (bounceSfx_) bounceSfx_->play();
    }

    float          screenW_, screenH_;
    float          dirX_, dirY_;
    AudioComponent* bounceSfx_{nullptr};
};

// ---------------------------------------------------------------------------

void GameScene::load() {
    setBackgroundColor(SDL_Color{0x1f, 0x1f, 0x1f, 0xff});
    addSubsystem<CollisionSystem>();

    // --- Audio ---
    auto audioObj = addObject("Audio");
    auto bgMusic  = audioObj->addComponent<MusicComponent>("BGMusic", "assets/Music.ogg");
    bgMusic->setGain(0.1);

    GameEngine& engine       = GameEngine::instance();
    TextureManager& texManager = TextureManager::instance();

    auto [screenWidth, screenHeight] = getScreenSize();

    // --- Player ---
    auto player = addObject("Player");
    player->setTag("player");
    player->transform.position = {screenWidth / 2.f, screenHeight / 2.f};

    auto* playerRender = player->addComponent<RenderComponent>("Player RenderComponent");
    playerRender->setTexture(texManager.load(engine.getRenderer(), "assets/fish-gold_112x112.png"));

    auto* playerCollider = player->addComponent<BoxColliderComponent>("Player Collider", Vec2F{100.f, 100.f});
    playerCollider->debugDraw = true;
    playerCollider->onEnter = [](ColliderComponent* other) {
        SDL_Log("Player collided with: %s", other->getOwner()->getName().c_str());
    };
    playerCollider->onExit = [](ColliderComponent* other) {
        SDL_Log("Player separated from: %s", other->getOwner()->getName().c_str());
    };

    player->addComponent<TransitionComponent>("ToMenu", "menu");

    addPlayerController<MyPlayerController>()->possess(player);

    // --- HUD ---
    TTF_Font* font = FontManager::instance().load("assets/Roboto-Regular.ttf", 16.f);
    auto hud = addObject("HUD");
    auto hudText = hud->addComponent<TextComponent>("HUD Text", "Move with WASD/Gamepad - ESC to exit.",
        font, SDL_Color{255, 255, 0, 255},Vec2F{10.f, 10.f});
    hudText->setPivot(Vec2F{0.f, 0.f});

    // --- Enemy ---
    auto enemy = addObject("Enemy");
    enemy->setTag("enemy");
    auto* enemyRender = enemy->addComponent<RenderComponent>("Enemy RenderComponent");
    enemyRender->setTexture(texManager.load(engine.getRenderer(), "assets/fish-pink_45x40.png"));
    enemy->transform = Transform::make_position(screenWidth / 2, screenHeight / 2);

    enemy->addComponent<AudioComponent>("Bounce SFX", "assets/Bounce.mp3", 0.8f);

    auto* enemyCollider = enemy->addComponent<CircleColliderComponent>("Enemy Collider", 18.f);
    enemyCollider->debugDraw = true;

    auto* enemyAnimator = enemy->addComponent<AnimatorComponent>("Enemy Animator", enemyRender);
    enemyAnimator->addAnimation(AnimationBuilder::fromStrip("swim", 5, 45, 40));
    enemyAnimator->play("swim");

    addPlayerController<MyEnemyBounceAI>(
        static_cast<float>(screenWidth),
        static_cast<float>(screenHeight)
    )->possess(enemy);

}
