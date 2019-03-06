#pragma once
#include "CoreTypes.h"
namespace LittleEngine
{
struct JobHandle
{
	s64 jobID = -1;

	JobHandle() = default;
	JobHandle(s64 jobID);

	operator s64() const;

	void Wait() const;
	bool IsRunning() const;
	bool IsCompleted() const;

	void Enqueue(const std::function<void()>& Task, const String& name = "", bool bSilent = false);
	void EnqueueEngine(const std::function<void()>& Task, const String& name);
};
} // namespace LittleEngine