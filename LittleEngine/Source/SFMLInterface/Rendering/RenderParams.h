#pragma once
#include "Utils/Vector2.h"

namespace sf {
	class RenderWindow;
}

namespace Game {
	class WindowController;

	struct RenderParams {
		Vector2 screenPosition;
		Fixed screenRotation;

		RenderParams(WindowController& controller);
		WindowController& GetWindowController();
	private:
		WindowController* windowController;
	};
}
