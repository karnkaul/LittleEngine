#pragma once
#include "CoreTypes.h"
#if ENABLED(PROFILER)
#include <thread>
#include "SimpleTime.h"
#include "SFMLAPI/Rendering/Colour.h"

#define PROFILE_START(id, colour) LittleEngine::Debug::Profiler::Start(id, colour)
#define PROFILE_STOP(id) LittleEngine::Debug::Profiler::Stop(id)

namespace LittleEngine
{
namespace Debug
{
namespace Profiler
{
void Init(std::thread::id eventThreadID);
void Toggle(bool bEnable);
void Cleanup();

void Tick(Time dt);
void Render();
void Reset();

// Returns ID of profiling instance; call EndProfile with it
void Start(const String& id, Colour colour);
// Pass ID returned by ProfileStart; returns false if invalid ID
void Stop(const String& id);
} // namespace Profiler
} // namespace Debug
} // namespace LittleEngine

#else
#define PROFILE_START(id, colour)
#define PROFILE_STOP(id)
#endif
