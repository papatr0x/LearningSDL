// Created by Patricio Palma on 09-03-26.

#include "GameScene.h"
#include "Animation.h"
#include "AnimatorComponent.h"
#include "AudioComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "FontManager.h"
#include "GameEngine.h"
#include "InputComponent.h"
#include "MusicComponent.h"
#include "RenderComponent.h"
#include "ScriptComponent.h"
#include "TextComponent.h"
#include "TextureManager.h"
#include "TransitionComponent.h"

void GameScene::load()  {
    backgroundColor = SDL_Color{0x1f, 0x1f, 0x1f, 0xff};
    addSubsystem<CollisionSystem>();

    // --- Audio ---
    auto audioObj = addObject<Object>("Audio");
    auto bgMusic = audioObj->addComponent<MusicComponent>("BGMusic", "assets/Music.ogg");
    bgMusic->setGain(0.1);

    GameEngine& engine = GameEngine::instance();
    TextureManager& texManager = TextureManager::instance();

    int screenWidth, screenHeight;
    SDL_GetWindowSize(GameEngine::instance().getWindow(), &screenWidth, &screenHeight);

    // --- Player ---
    auto player  = addObject<Object>("Player");
    player->setTag("player");
    player->transform.position = {screenWidth/2.f, screenHeight/2.f};
    auto* inputComp = player->addComponent<InputComponent>("Player InputComponent");
    inputComp->bind("MoveUp",    SDL_SCANCODE_W);
    inputComp->bind("MoveDown",  SDL_SCANCODE_S);
    inputComp->bind("MoveLeft",  SDL_SCANCODE_A);
    inputComp->bind("MoveRight", SDL_SCANCODE_D);
    inputComp->bind("Exit",      SDL_SCANCODE_ESCAPE);

    auto playerRenderComp = player->addComponent<RenderComponent>("Player RenderComponent");
    playerRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-gold_112x112.png"));

    auto* playerCollider = player->addComponent<BoxColliderComponent>("Player Collider", Vec2F{100.f, 100.f});
    playerCollider->debugDraw = true;
    playerCollider->onEnter = [](ColliderComponent* other) {
        SDL_Log("Player collided with: %s", other->getOwner()->getName().c_str());
    };
    playerCollider->onExit = [](ColliderComponent* other) {
        SDL_Log("Player separated from: %s", other->getOwner()->getName().c_str());
    };

    auto tc = player->addComponent<TransitionComponent>("ToMenu","menu");
    player->addComponent<ScriptComponent>("Player Script", [inputComp, tc](const float dt, Object* owner) {
        if (!inputComp) {
            SDL_Log("ScriptComponent::lambdaScript - InputComponent is null");
            return;
        }

        // Move to the mouse position
        if (inputComp->isMouseButtonPressed(SDL_BUTTON_LEFT)) {
            owner->transform.position = owner->getScene()->screenToWorld(inputComp->getMousePosition());
        }
        constexpr float speed = 100.0f;

        // WASD movement
        if (inputComp->isActionDown("MoveUp"))    owner->transform.position.y -= speed * dt;
        if (inputComp->isActionDown("MoveDown"))  owner->transform.position.y += speed * dt;
        if (inputComp->isActionDown("MoveLeft"))  owner->transform.position.x -= speed * dt;
        if (inputComp->isActionDown("MoveRight")) owner->transform.position.x += speed * dt;
        if (inputComp->isActionDown("Exit"))      tc->trigger();

        // Left analog stick
        owner->transform.position.x += inputComp->getAxis(SDL_GAMEPAD_AXIS_LEFTX) * speed * dt;
        owner->transform.position.y += inputComp->getAxis(SDL_GAMEPAD_AXIS_LEFTY) * speed * dt;
    });

    // --- HUD ---
    TTF_Font* font = FontManager::instance().load("assets/Roboto-Regular.ttf", 16.f);
    auto hud = addObject<Object>("HUD");
    auto hudText = hud->addComponent<TextComponent>("HUD Text", "Move with WASD/Gamepad - ESC to exit.", font, Vec2F{10.f, 10.f}, SDL_Color{255, 255, 0, 255});
    hudText->setPivot(Vec2F{0.f, 0.f});

    // --- Enemy ---
    auto enemy  = addObject<Object>("Enemy");
    enemy->setTag("enemy");
    auto* enemyRenderComp = enemy->addComponent<RenderComponent>("Enemy RenderComponent");
    enemyRenderComp->setTexture(texManager.load(engine.getRenderer(), "assets/fish-pink_45x40.png"));
    enemy->transform = Transform::make_position(screenWidth/2, screenHeight/2);

    struct EnemyState { float dirX, dirY; };
    auto state = std::make_shared<EnemyState>(
        (rand() > RAND_MAX/2) ? 1.f : -1.f,
        (rand() > RAND_MAX/2) ? 1.f : -1.f
    );

    // Add sound effect on bounce
    auto bounceSfx = enemy->addComponent<AudioComponent>("Bounce SFX", "assets/Bounce.mp3", 0.8f);

    auto* enemyCollider = enemy->addComponent<CircleColliderComponent>("Enemy Collider", 18.f);
    enemyCollider->debugDraw = true;
    enemyCollider->onEnter = [state, enemy, bounceSfx](ColliderComponent* other) {
        if (other->getOwner()->getTag() != "player") return;

        // Collision normal: from player center to enemy center.
        const Vec2F n_raw = enemy->transform.position - other->getOwner()->transform.position;
        const float len = std::sqrt(n_raw.x * n_raw.x + n_raw.y * n_raw.y);
        if (len == 0.f) return;
        const Vec2F n = n_raw * (1.f / len);

        // Reflect velocity: v' = v - 2(v·n)n
        const float dot = state->dirX * n.x + state->dirY * n.y;
        state->dirX -= 2.f * dot * n.x;
        state->dirY -= 2.f * dot * n.y;

        bounceSfx->play();
    };

    auto* enemyAnimator = enemy->addComponent<AnimatorComponent>("Enemy Animator", enemyRenderComp);
    enemyAnimator->addAnimation(AnimationBuilder::fromStrip("swim", 5, 45, 40));
    enemyAnimator->play("swim");

    enemy->addComponent<ScriptComponent>("Enemy Script",
        [width=static_cast<float>(screenWidth), height=static_cast<float>(screenHeight), state, bounceSfx]
        (float dt, Object* enemy) {
            constexpr float speed = 100.f;
            Vec2F& pos = enemy->transform.position;
            pos.x += state->dirX * speed * dt;
            pos.y += state->dirY * speed * dt;

            bool bounced = false;
            if (pos.x > width  || pos.x < 0.f) {
                state->dirX = -state->dirX;
                bounced = true;
            }
            if (pos.y > height || pos.y < 0.f) {
                state->dirY = -state->dirY;
                bounced = true;
            }
            if (bounced)
                bounceSfx->play();
    });

    // --- Camera controller ---
    auto* camCtrl = addObject<Object>("CameraController");
    auto* camInput = camCtrl->addComponent<InputComponent>("CameraInput");
    camInput->bind("CameraToggle", SDL_SCANCODE_P);

    // 0 = Normal, 1 = Player, 2 = Enemy
    auto camMode = std::make_shared<int>(0);
    camCtrl->addComponent<ScriptComponent>("CameraScript",
        [camInput, player, enemy, camMode](float /*dt*/, Object* owner) {
            if (camInput->isActionPressed("CameraToggle"))
                *camMode = (*camMode + 1) % 3;

            if (*camMode == 1)
                owner->getScene()->getCamera().position = player->transform.position;
            else if (*camMode == 2)
                owner->getScene()->getCamera().position = enemy->transform.position;
        });
}
