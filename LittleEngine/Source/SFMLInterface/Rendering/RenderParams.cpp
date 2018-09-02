#include "stdafx.h"
#include "RenderParams.h"
#include "SFMLInterface/WindowController.h"

namespace LittleEngine {
	RenderParams::RenderParams(WindowController& controller) {
		this->windowController = &controller;
	}

	WindowController& RenderParams::GetWindowController() {
		return *windowController;
	}
}
