#include "Profiler.h"
#if ENABLED(PROFILER)
#include "Core/LECoreGame/LECoreUtils/Logger.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Context/LEContext.h"

namespace LittleEngine::Debug::Profiler
{
Time maxTickDeltaTime;
std::mutex entriesMutex;
std::unordered_map<std::string, Entry> entries;

Entry::Entry(std::string id, Colour colour, Time startTime, Time maxTime, bool bCustom)
	: id(std::move(id)), colour(colour), startTime(startTime), maxTime(maxTime), bCustom(bCustom)
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

void StartTicked(std::string id, Colour colour)
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

void StartCustom(std::string id, Time maxTime, Colour colour)
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

void Stop(std::string id)
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
} // namespace LittleEngine::Debug::Profiler
#endif
