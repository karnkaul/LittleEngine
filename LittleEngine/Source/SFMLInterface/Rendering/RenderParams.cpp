#include "le_stdafx.h"
#include "RenderParams.h"
#include "SFMLInterface/WindowController.h"

namespace LittleEngine {
	RenderParams::RenderParams(WindowController& controller) : windowController(&controller) {
		Reset();
	}

	WindowController& RenderParams::GetWindowController() {
		return *windowController;
	}
	
	void RenderParams::Reset() {
		screenPosition = Vector2::Zero;
		screenRotation = Fixed::Zero;
		screenScale = Vector2::One;
	}
}
