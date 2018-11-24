#pragma once
#include "Vector2.h"
#include "UByte.h"

namespace LittleEngine {
	class WindowController;

	using Vector2 = GameUtils::Vector2;
	using Fixed = GameUtils::Fixed;
	using UByte = GameUtils::UByte;

	// \brief Compressed wrapper struct for SFML Color
	struct Colour {
	public:
		UByte r;
		UByte g;
		UByte b;
		UByte a;

		// Warning: All inputs must be unsigned ints!
		Colour(UByte r = 255, UByte g = 255, UByte b = 255, UByte a = 255) : r(r), g(g), b(b), a(a) {}

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
		FX = 60,
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
		Vector2 screenPosition = Vector2::Zero;
		Fixed screenRotation = Fixed::Zero;
		Vector2 screenScale = Vector2::One;

		RenderParams(WindowController& controller);

		WindowController& GetWindowController();

	private:
		WindowController* windowController;
	};
}
