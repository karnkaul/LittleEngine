#pragma once
#include "Core/Jobs/JobHandle.h"
#include "Core/Jobs/JobCatalog.h"

namespace Core
{
namespace Jobs
{
void Init(u32 workerCount);
void Cleanup();

JobHandle Enqueue(Task task, std::string name = "", bool bSilent = false);
JobCatalog* CreateCatalog(std::string name);
void ForEach(std::function<void(size_t)> indexedTask, size_t iterationCount, size_t iterationsPerJob, size_t startIdx = 0);

void Update();
bool AreWorkersIdle();
} // namespace Jobs
} // namespace Core
