#include "stdafx.h"
#include "JobHandle.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
JobHandle::JobHandle(s64 jobID) : jobID(jobID)
{
}

JobHandle::operator s64() const
{
	return jobID;
}

void JobHandle::Wait() const
{
	Services::Jobs()->Wait(jobID);
}

bool JobHandle::IsRunning() const
{
	return Services::Jobs()->IsRunning(jobID);
}
bool JobHandle::IsCompleted() const
{
	return Services::Jobs()->IsCompleted(jobID);
}

void JobHandle::Enqueue(const std::function<void()>& Task, const String& name, bool bSilent)
{
	*this = Services::Jobs()->Enqueue(Task, name, bSilent);
}

void JobHandle::EnqueueEngine(const std::function<void()>& Task, const String& name)
{
	*this = Services::Jobs()->EnqueueEngine(Task, name);
}
} // namespace LittleEngine