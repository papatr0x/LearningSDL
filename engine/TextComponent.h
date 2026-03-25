// Created by Patricio Palma on 08-03-26.

#ifndef LETSLEARNSDL_TEXTCOMPONENT_H
#define LETSLEARNSDL_TEXTCOMPONENT_H

#include "Component.h"
#include "Object.h"
#include "Scene.h"
#include "Vector.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

class TextComponent : public Component {
public:
    TextComponent(Object* owner, const std::string& id, std::string text, TTF_Font* font,
        SDL_Color color = {0xff, 0xff, 0xff, 0xff}, Vec2F offset = {})
        : Component(owner, id), text(std::move(text)), font(font), color(color), offset(offset) {
        screenSpace = true;
    }

    ~TextComponent() {
        if (ttfText) TTF_DestroyText(ttfText);
    }

    void onStart() noexcept override {
        if (!font) {
            SDL_Log("TextComponent '%s': font is null", name.c_str());
            return;
        }
        auto engine = owner->getScene()->getTextEngine();
        ttfText = TTF_CreateText(engine, font, text.c_str(), 0);
        if (!ttfText) {
            SDL_Log("TextComponent '%s': TTF_CreateText failed: %s", name.c_str(), SDL_GetError());
            return;
        }
        TTF_SetTextColor(ttfText, color.r, color.g, color.b, color.a);
    }

    void render(SDL_Renderer*) noexcept override {
        if (!ttfText) return;
        int w{}, h{};
        TTF_GetTextSize(ttfText, &w, &h);
        const Vec2F camOff = screenSpace ? Vec2F{} : owner->getScene()->getWorldRenderOffset();
        const float x = owner->transform.position.x + offset.x - pivot.x * w + camOff.x;
        const float y = owner->transform.position.y + offset.y - pivot.y * h + camOff.y;
        TTF_DrawRendererText(ttfText, x, y);
    }

    void setText(std::string newText) {
        text = std::move(newText);
        if (ttfText) TTF_SetTextString(ttfText, text.c_str(), 0);
    }

    void setColor(SDL_Color c) {
        color = c;
        if (ttfText) TTF_SetTextColor(ttfText, c.r, c.g, c.b, c.a);
    }

    void setPivot(Vec2F p) { pivot = p; }
    void setOffset(Vec2F o) { offset = o; }
    const std::string& getText() const { return text; }

private:
    std::string text;
    TTF_Font*   font{};
    SDL_Color   color;
    Vec2F       pivot{0.5f, 0.5f};
    Vec2F       offset{};
    TTF_Text*   ttfText{};
};

#endif //LETSLEARNSDL_TEXTCOMPONENT_H
