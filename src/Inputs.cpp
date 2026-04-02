#include "Inputs.h"


namespace inputs {
    bool keys[(int)Keys::LAST_KEY];
	bool lastKeys[(int)Keys::LAST_KEY];

	glm::vec2 mousePos;
	glm::vec2 lastMousePos;
	glm::vec2 mouseDelta;

    void newFrame() {
		for (int i = 0; i < (int)Keys::LAST_KEY; i++) {
			lastKeys[i] = false;
			lastKeys[i] |= keys[i];
		}
    }

	glm::vec2 getMousePos() { return mousePos; }
	glm::vec2 getMouseDelta() { return mousePos - lastMousePos; }

	void setMousePos(glm::vec2 pos) {
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