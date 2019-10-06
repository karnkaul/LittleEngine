#include "Jobs.h"
#include "LEMacros.h"
#include "Jobs/JobManager.h"
#include "OS.h"

namespace Core
{
namespace
{
std::unique_ptr<JobManager> uManager;
}

namespace Jobs
{
JobManager* g_pJobManager = nullptr;

void Init(u32 workerCount)
{
	uManager = std::make_unique<JobManager>(workerCount, OS::Threads::VacantThreadCount());
	g_pJobManager = uManager.get();
}

void Cleanup()
{
	uManager = nullptr;
	g_pJobManager = nullptr;
}

JobHandle Enqueue(Task task, std::string name /* = "" */, bool bSilent /* = false */)
{
	Assert(uManager, "JobManager is null!");
	return uManager->Enqueue(std::move(task), name, bSilent);
}

JobCatalog* CreateCatalog(std::string name)
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
