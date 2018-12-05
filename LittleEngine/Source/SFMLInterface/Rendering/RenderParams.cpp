#include "le_stdafx.h"
#include "RenderParams.h"
#include "SFMLInterface/WindowController.h"

namespace LittleEngine {
	RenderParams::RenderParams(WindowController& controller) : pWindowController(&controller) {
		Reset();
	}

	WindowController& RenderParams::GetWindowController() {
		return *pWindowController;
	}
	
	void RenderParams::Reset() {
		screenPosition = Vector2::Zero;
		screenRotation = Fixed::Zero;
		screenScale = Vector2::One;
	}
}
