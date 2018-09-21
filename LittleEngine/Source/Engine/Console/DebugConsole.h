#pragma once
#include "Engine/Engine.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine { namespace DebugConsole {
	bool IsActive();
	void Activate(bool setActive);
	void UpdateInput(const WindowController& windowController);
	void RenderConsole(const Engine& engine, RenderParams& params);
	void Cleanup();
} }
