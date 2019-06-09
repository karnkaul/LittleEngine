#include "stdafx.h"
#include "Profiler.h"
#if ENABLED(PROFILER)
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "LittleEngine/Debug/Tweakable.h"
#include "LittleEngine/Context/LEContext.h"

namespace LittleEngine
{
namespace Debug
{
namespace Profiler
{
Time maxTickDeltaTime;
std::mutex entriesMutex;
UMap<String, Entry> entries;

using Lock = std::lock_guard<std::mutex>;

Entry::Entry(String id, Colour colour, Time startTime, Time maxTime, bool bCustom)
	: id(std::move(id)),
	  colour(colour),
	  startTime(startTime),
	  maxTime(maxTime),
	  bCustom(bCustom)
{
}

Time Entry::Elapsed() const
{
	return endTime - startTime;
}
Fixed Entry::Ratio() const
{
	return Fixed((endTime - startTime).AsSeconds() / maxTime.AsSeconds());
}

void StartTicked(String id, Colour colour)
{
	Lock lock(entriesMutex);
	auto search = entries.find(id);
	if (search != entries.end())
	{
		search->second.startTime = Time::Now();
		search->second.endTime = Time::Now();
	}
	else
	{
		Entry newEntry(id, colour, Time::Now(), maxTickDeltaTime, false);
		entries.emplace(std::move(id), std::move(newEntry));
	}
}

void StartCustom(String id, Time maxTime, Colour colour)
{
	Lock lock(entriesMutex);
	auto search = entries.find(id);
	if (search != entries.end())
	{
		search->second.startTime = Time::Now();
		search->second.endTime = Time::Now();
	}
	else
	{
		Entry newEntry(id, colour, Time::Now(), maxTime, true);
		entries.emplace(std::move(id), std::move(newEntry));
	}
}

void Stop(String id)
{
	Lock lock(entriesMutex);
	auto search = entries.find(id);
	if (search != entries.end())
	{
		Entry& entry = search->second;
		entry.endTime = Time::Now();
		entry.timeRatio = entry.Ratio();
	}
}
} // namespace Profiler
} // namespace Debug
} // namespace LittleEngine
#endif
