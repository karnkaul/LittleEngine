#pragma once
#include "Core/Jobs/JobHandle.h"
#include "Core/Jobs/MultiJob.h"

namespace Core
{
namespace Jobs
{
void Init(u32 workerCount);
void Cleanup();

JobHandle Enqueue(Task task, String name = "", bool bSilent = false);
MultiJob* CreateMultiJob(String name);
void Update();
bool AreWorkersIdle();
} // namespace Jobs
} // namespace Core