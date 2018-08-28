#pragma once
#include "Utils/Vector2.h"

namespace Game {
	class WindowController;

	// Assumes availability of at least 100 layers!
	enum class LayerID {
		Base = 0,
		Default = 10,
		Enemy = 30,
		Player = 50,
		UI = 70,
		Collider = 80,
		TOP = 90
	};

	struct LayerInfo {
	public:
		LayerInfo() = default;
		explicit LayerInfo(int layerID);
		LayerInfo(LayerID layerID);

		int GetLayerID();
		int SetLayerID(int layerID);
		int SetLayerID(LayerID layerID);

	private:
		int layerID = static_cast<int>(LayerID::Default);
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
