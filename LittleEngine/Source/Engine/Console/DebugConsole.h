#pragma once
#include "Engine/Engine.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine { namespace DebugConsole {
	extern const Colour LOG_BG_COLOUR;
	extern const Colour LOG_TEXT_COLOUR;
	extern const Colour LOG_QUERY_COLOUR;
	extern const Colour LOG_WARNING_COLOUR;

	void Init(Engine& engine);
	bool IsActive();
	void Activate(bool bSetActive);
	void UpdateInput(const RawTextInput& windowController);
	void RenderConsole(const Engine& engine, RenderParams& params, Fixed deltaTime);
	void Cleanup();
} }
