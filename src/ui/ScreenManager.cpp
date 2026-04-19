#include "ScreenManager.h"

#include <unordered_map>
#include <stack>

#include "ScreenBase.h"
#include "Window.h"
#include "screens/StartScreens.h"
#include "render/SpritesRenderer.h"
#include "Game.h"


namespace ui::screenManager {
	std::unordered_map<ScreenId, ScreenBase*> screens;
	std::stack<ScreenBase*> menuScreensStack;
	std::stack<ScreenBase*> inWorldScreensStack;

	ScreenBase* currentScreen = nullptr;

	rk::SpritesRenderer spritesRenderer;

	void changeLogic();

	void add(ScreenBase* screen) { screens.emplace(screen->id, screen); }

	void start() {
		add(new StartScreen(ScreenId::StartScreen));

		spritesRenderer.start();

		change(ScreenId::StartScreen);
	}

	bool currentScreenIs(ScreenId id) { return currentScreen == screens.at(id); }

	void returnBack() {
		if (!currentScreen->canReturn)
			return;

		if (game::inWorld) {
			if (inWorldScreensStack.size() <= 1)
				return;

			inWorldScreensStack.pop();
			currentScreen = inWorldScreensStack.top();
		}
		else {
			if (menuScreensStack.size() <= 1)
				return;

			menuScreensStack.pop();
			currentScreen = menuScreensStack.top();
		}

		changeLogic();
	}

	void change(ScreenId id) {
		auto newScreen = screens[id];

		if (currentScreen == newScreen)
			return;

		currentScreen = newScreen;

		if (game::inWorld)
			inWorldScreensStack.push(currentScreen);
		else
			menuScreensStack.push(currentScreen);

		changeLogic();
	}


	void updateUi(f32 dt) {
		currentScreen->update(dt);
	}

	void drawUi() {
		currentScreen->draw(spritesRenderer);

		spritesRenderer.draw();
	}

	void resize(i32 width, i32 height) {
		Vec2 screenSize = { (f32)width, (f32)height };

		currentScreen->screenSize = screenSize;
		currentScreen->screenCenter = screenSize / 2.f;
		currentScreen->resize();
	}

	void changeLogic() {
		Vec2 screenSize = { (f32)window::getWindowWidth(), (f32)window::getWindowHeight() };

		if (!currentScreen->started) {
			currentScreen->started = true;
			currentScreen->start();
			currentScreen->resize();
		}

		if (currentScreen->screenSize != screenSize) {
			currentScreen->screenSize = screenSize;
			currentScreen->screenCenter = screenSize / 2.f;
			currentScreen->resize();
		}
	}
}