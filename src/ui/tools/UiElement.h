#pragma once

#include "math/Vec2.h"


namespace ui::tools {
    class UiElement {
    public:
        virtual Vec2 getPos() const { return m_pos; }
        virtual Vec2 getSize() const { return m_size; }

        virtual void setPos(Vec2 pos) = 0;
        void setPosX(f32 x) { setPos({ x, getPos().y }); }
        void setPosY(f32 y) { setPos({ getPos().x, y }); }

        virtual void setSize(Vec2 size) = 0;
        void setSizeX(f32 x) { setSize({ x, getSize().y }); }
        void setSizeY(f32 y) { setSize({ getSize().x, y }); }

        bool mouseHover(Vec2 mousePos) const {
            return mousePos.x > getPos().x &&
                   mousePos.x < getPos().x + getSize().x &&
                   mousePos.y > getPos().y &&
                   mousePos.y < getPos().y + getSize().y;
        }

        Vec2 getCenter(const UiElement& other) const { return other.getPos() + other.getSize() / 2.f - getSize() / 2.f; }
        f32 getCenterX(const UiElement& other) const { return other.getPos().x + other.getSize().x / 2.f - getSize().x / 2.f; }
        f32 getCenterY(const UiElement& other) const { return other.getPos().y + other.getSize().y / 2.f - getSize().y / 2.f; }

        void setCenter(const UiElement& other) { setPos(other.getPos() + other.getSize() / 2.f - getSize() / 2.f); }
        void setCenterX(const UiElement& other) { setPosX(other.getPos().x + other.getSize().x / 2.f - getSize().x / 2.f); }
        void setCenterY(const UiElement& other) { setPosY(other.getPos().y + other.getSize().y / 2.f - getSize().y / 2.f); }


        Vec2 getFinal() const { return getPos() + getSize(); }
        f32 getFinalX() const { return getPos().x + getSize().x; }
        f32 getFinalY() const { return getPos().y + getSize().y; }

        void setFinal(const UiElement& sprite) { setPos(sprite.getPos() + sprite.getSize()); }
        void setFinalX(const UiElement& sprite) { setPosY(sprite.getPos().x + sprite.getSize().x); }
        void setFinalY(const UiElement& sprite) { setPosY(sprite.getPos().y + sprite.getSize().y); }

    protected:
        Vec2 m_pos;
        Vec2 m_size;
    };
}