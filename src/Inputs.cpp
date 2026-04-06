#include "Inputs.h"


namespace inputs {
    bool keys[(int)Keys::LAST_KEY];
	bool lastKeys[(int)Keys::LAST_KEY];

	Vec2 mousePos;
	Vec2 lastMousePos;
	Vec2 mouseDelta;

    void newFrame() {
		inputs::setMousePos(mousePos);

		for (int i = 0; i < (int)Keys::LAST_KEY; i++) {
			lastKeys[i] = false;
			lastKeys[i] |= keys[i];
		}
    }

	Vec2 getMousePos() { return mousePos; }
	Vec2 getMouseDelta() { return mousePos - lastMousePos; }

	void setMousePos(Vec2 pos) {
    	lastMousePos = mousePos;
	    mousePos = pos;
    }

    bool isKeyDown(Keys key)    { return  keys[(int)key] &&  lastKeys[(int)key]; }
	bool isKeyPressed(Keys key) { return  keys[(int)key] && !lastKeys[(int)key]; }
	bool isKeyRelease(Keys key) { return !keys[(int)key] &&  lastKeys[(int)key]; }
	void setKeyState(int key, bool state) { keys[key] = state; }

	bool isMouseButtonDown(MouseButton button)    { return  keys[(int)button] &&  lastKeys[(int)button]; }
	bool isMouseButtonPressed(MouseButton button) { return  keys[(int)button] && !lastKeys[(int)button]; }
	bool isMouseButtonRelease(MouseButton button) { return !keys[(int)button] &&  lastKeys[(int)button]; }
	void setMouseButtonState(int button, bool state) { keys[button] = state; }
}