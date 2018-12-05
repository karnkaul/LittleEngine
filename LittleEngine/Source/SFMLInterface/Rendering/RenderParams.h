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
		static const Colour Black;
		static const Colour White;
		static const Colour Red;
		static const Colour Green;
		static const Colour Blue;
		static const Colour Yellow;
		static const Colour Magenta;
		static const Colour Cyan;
		static const Colour Transparent;

		UByte r;
		UByte g;
		UByte b;
		UByte a;

		// Warning: All inputs must be unsigned ints!
		Colour(UByte r = 255, UByte g = 255, UByte b = 255, UByte a = 255) : r(r), g(g), b(b), a(a) {}
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
	private:
		int layerID = static_cast<int>(LayerID::Default);

	public:
		LayerInfo() = default;
		explicit LayerInfo(int layerID);
		LayerInfo(const LayerID& layerID);

		int GetLayerID() const;
		int SetLayerID(int layerID);
		int SetLayerID(const LayerID& layerID);
	};

	// \brief Final parameters passed to SFML RenderWindow
	struct RenderParams {
		Vector2 screenPosition;
		Vector2 screenScale;
		Fixed screenRotation;
	private:
		WindowController* pWindowController;

	public:
		RenderParams(WindowController& controller);

		WindowController& GetWindowController();
		void Reset();
	};
}
