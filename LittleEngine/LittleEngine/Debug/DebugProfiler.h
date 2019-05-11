#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(PROFILER)
#include <thread>
#include "Core/SimpleTime.h"
#include "SFMLAPI/Rendering/Colour.h"

#define PROFILE_START(id, colour) Debug::Profiler::StartTicked(id, colour)
#define PROFILE_FRAME(id, colour) Debug::Profiler::StartFramed(id, colour)
#define PROFILE_STOP(id) Debug::Profiler::Stop(id)

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
void Reset();

void StartTicked(String id, Colour colour);
void StartFramed(String id, Colour colour);
void Stop(String id);
} // namespace Profiler
} // namespace Debug
} // namespace LittleEngine

#else
#define PROFILE_START(id, colour)
#define PROFILE_FRAME(id, colour) 
#define PROFILE_STOP(id)
#endif
