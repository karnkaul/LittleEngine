#include "stdafx.h"
#include "Jobs.h"
#include "Core/Jobs/JobManager.h"

namespace Core
{
namespace
{
UPtr<JobManager> uManager;
}

namespace Jobs
{
JobManager* g_pJobManager = nullptr;

void Init(u32 workerCount)
{
	uManager = MakeUnique<JobManager>(workerCount);
	g_pJobManager = uManager.get();
}

void Cleanup()
{
	uManager = nullptr;
	g_pJobManager = nullptr;
}

JobHandle Enqueue(Task task, String name /* = "" */, bool bSilent /* = false */)
{
	Assert(uManager, "JobManager is null!");
	return uManager->Enqueue(std::move(task), name, bSilent);
}

JobCatalog* CreateCatalog(String name)
{
	Assert(uManager, "JobManager is null!");
	return uManager->CreateCatalog(std::move(name));
}

void ForEach(std::function<void(size_t)> indexedTask, size_t iterationCount, size_t iterationsPerJob, size_t startIdx)
{
	Assert(uManager, "JobManager is null!");
	uManager->ForEach(indexedTask, iterationCount, iterationsPerJob, startIdx);
}

void Update()
{
	if (uManager)
	{
		uManager->Update();
	}
}

bool AreWorkersIdle()
{
	return uManager ? uManager->AreWorkersIdle() : true;
}
} // namespace Jobs
} // namespace Core