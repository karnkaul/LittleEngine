#pragma once
#include "Engine/Engine.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine { namespace DebugConsole {
	void Init(Engine& engine);
	bool IsActive();
	void Activate(bool setActive);
	void UpdateInput(const RawTextInput& windowController);
	void RenderConsole(const Engine& engine, RenderParams& params, Fixed deltaTime);
	void Cleanup();
} }
