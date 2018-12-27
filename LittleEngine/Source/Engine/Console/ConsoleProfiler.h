#pragma once
#include <string>
#include "SFMLInterface/Rendering/RenderParams.h"
#include "Engine/CoreGame.hpp"

#if ENABLED(DEBUG_PROFILER)
#define PROFILE_START(id, colour) LittleEngine::DebugConsole::Profiler::Start(id, colour)
#define PROFILE_STOP(id) LittleEngine::DebugConsole::Profiler::Stop(id)
#else
#define PROFILE_START(id, colour)
#define PROFILE_STOP(id)
#endif

namespace LittleEngine { 
	class Engine;
namespace DebugConsole {
	namespace Profiler {
#if ENABLED(DEBUG_PROFILER)
		void Init(Engine& engine);
		void Toggle(bool bEnable);
		void Cleanup();
		
		void Tick(const Fixed& deltaMS);
		void Render();
		void Reset();

		// Returns ID of profiling instance; call EndProfile with it
		void Start(const std::string& id, Colour colour);
		// Pass ID returned by ProfileStart; returns false if invalid ID
		void Stop(const std::string& id);
#endif
	}
} }
