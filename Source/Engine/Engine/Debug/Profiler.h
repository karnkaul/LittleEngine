#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(PROFILER)
#include <thread>
#include "Core/LECoreGame/LECoreUtils/SimpleTime.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/LayerID.h"

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
struct Entry
{
	std::string id;
	Fixed timeRatio;
	Colour colour;
	Time startTime = Time::Zero;
	Time endTime = Time::Zero;
	Time maxTime = Time::Milliseconds(1);
	bool bCustom;

	Entry(std::string id, Colour colour, Time startTime, Time maxTime, bool bCustom);
	Time Elapsed() const;
	Fixed Ratio() const;
};

extern std::mutex entriesMutex;
extern std::unordered_map<std::string, Entry> entries;
extern Time maxTickDeltaTime;
void StartTicked(std::string id, Colour colour);
void StartCustom(std::string id, Time maxTime, Colour colour);
void Stop(std::string id);
} // namespace Profiler
} // namespace Debug
} // namespace LittleEngine

#else
#define PROFILE_START(id, colour)
#define PROFILE_CUSTOM(id, maxTime, colour)
#define PROFILE_STOP(id)
#endif
