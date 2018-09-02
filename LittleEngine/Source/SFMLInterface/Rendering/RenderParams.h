#pragma once
#include "Utils/Vector2.h"

namespace LittleEngine {
	class WindowController;

	using Vector2 = Utils::Vector2;
	using Fixed = Utils::Fixed;

	// \brief Shorthand codes for layer spaces
	// Note: Assumes availability of at least 100 layers!
	enum class LayerID {
		Base = 0,
		Default = 10,
		Enemy = 30,
		Player = 50,
		UI = 70,
		Collider = 80,
		TOP = 90
	};

	// \brief Safe Wrapper for LayerID (WindowController::Buffer compatible)
	struct LayerInfo {
	public:
		LayerInfo() = default;
		explicit LayerInfo(int layerID);
		LayerInfo(const LayerID& layerID);

		int GetLayerID();
		int SetLayerID(int layerID);
		int SetLayerID(const LayerID& layerID);

	private:
		int layerID = static_cast<int>(LayerID::Default);
	};

	// \brief Final parameters passed to SFML RenderWindow
	struct RenderParams {
		Vector2 screenPosition;
		Fixed screenRotation;

		RenderParams(WindowController& controller);

		WindowController& GetWindowController();

	private:
		WindowController* windowController;
	};
}
