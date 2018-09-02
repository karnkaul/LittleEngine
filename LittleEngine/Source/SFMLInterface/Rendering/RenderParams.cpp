#include "stdafx.h"
#include "RenderParams.h"
#include "SFMLInterface/WindowController.h"

namespace Game {
	RenderParams::RenderParams(WindowController& controller) {
		this->windowController = &controller;
	}

	WindowController& RenderParams::GetWindowController() {
		return *windowController;
	}
}
