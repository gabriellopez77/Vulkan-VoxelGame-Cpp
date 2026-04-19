#include "StartScreens.h"

#include "ui/tools/Sprites.h"


ui::tools::Sprite sprite1;
ui::tools::Sprite sprite2;

void ui::StartScreen::start() {
	sprite1.setPos({ 0, 10 });
	sprite1.setSize({ 10, 20 });
	sprite1.color = { 0, 0, 128, 255 };

	sprite2.setPos({ 300, 50 });
	sprite2.setSize({ 90, 40 });
	sprite2.color = { 97, 34, 128, 255 };
}

void ui::StartScreen::update(f32 dt) {

}

void ui::StartScreen::draw(rk::SpritesRenderer& renderer) {
	sprite1.draw(renderer);
	sprite2.draw(renderer);
}

void ui::StartScreen::resize() {

}