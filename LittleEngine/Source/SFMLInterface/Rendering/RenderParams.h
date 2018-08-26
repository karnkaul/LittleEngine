#pragma once
#include "Utils/Vector2.h"

namespace sf {
	class RenderWindow;
}

namespace Game {
	class WindowController;

	struct LayerInfo {
	public:
		LayerInfo() = default;
		LayerInfo(int layerID);

		int GetLayerID();
		int SetLayerID(int layerID);

	private:
		int layerID = 0;
	};

	struct RenderParams {
		Vector2 screenPosition;
		Fixed screenRotation;

		RenderParams(WindowController& controller);
		WindowController& GetWindowController();
	private:
		WindowController& windowController;
	};
}
