#pragma once
#include "Utils/Vector2.h"

namespace LittleEngine {
	class WindowController;

	using Vector2 = Utils::Vector2;
	using Fixed = Utils::Fixed;

	// \brief Wrapper struct for SFML Color
	struct Colour {
	public:
		Fixed r;
		Fixed g;
		Fixed b;
		Fixed a;

		Colour(Fixed r = 255, Fixed g = 255, Fixed b = 255, Fixed a = 255) : r(r), g(g), b(b), a(a) {}

		const static Colour Black;
		const static Colour White;
		const static Colour Red;
		const static Colour Green;
		const static Colour Blue;
		const static Colour Yellow;
		const static Colour Magenta;
		const static Colour Cyan;
		const static Colour Transparent;
	};

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
