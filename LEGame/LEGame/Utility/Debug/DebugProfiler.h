#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(PROFILER)
#include <thread>
#include "Core/SimpleTime.h"
#include "SFMLAPI/Rendering/Colour.h"

#define PROFILE_START(id, colour) Debug::Profiler::StartTicked(id, colour)
#define PROFILE_CUSTOM(id, maxTime, colour) Debug::Profiler::StartCustom(id, maxTime, colour)
#define PROFILE_STOP(id) Debug::Profiler::Stop(id)

namespace LittleEngine
{
class LEContext;

namespace Debug
{
namespace Profiler
{
void Init(LEContext& context, Time maxTickTime);
void Toggle(bool bEnable);
void Cleanup();

void Tick(Time dt);
} // namespace Profiler
} // namespace Debug
} // namespace LittleEngine

#else
#define PROFILE_START(id, colour)
#define PROFILE_CUSTOM(id, maxTime, colour) 
#define PROFILE_STOP(id)
#endif
