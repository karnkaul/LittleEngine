#pragma once
#include <list>
#include <memory>
#include <optional>
#include "Core/CoreTypes.h"
#include "JobHandle.h"

namespace Core
{

class JobManager final
{
public:
	static constexpr s32 INVALID_ID = -1;

private:
	using Task = std::function<void()>;

	class Job
	{
	private:
		std::promise<void> m_promise;

	public:
		std::string m_logName;
		std::string m_exception;
		JobHandle m_sHandle;
		Task m_task;
		s64 m_id;
		bool m_bSilent = false;
		
	public:
		Job();
		Job(s64 id, Task task, std::string name, bool bSilent);
		Job(Job&& rhs) = default;
		Job& operator=(Job&& rhs) = default;

		void Run();
		void Fulfil();
	};

private:
	std::vector<std::unique_ptr<class JobWorker>> m_jobWorkers;
	std::list<std::unique_ptr<class JobCatalog>> m_catalogs;
	std::list<Job> m_jobQueue;
	mutable std::mutex m_queueMutex;
	s64 m_nextGameJobID = 0;

public:
	JobManager(u32 workerCount);
	~JobManager();

public:
	JobHandle Enqueue(Task task, std::string name = "", bool bSilent = false);
	JobCatalog* CreateCatalog(std::string name);
	void ForEach(std::function<void(size_t)> indexedTask, size_t iterationCount, size_t iterationsPerJob, size_t startIdx = 0);

	void Update();
	bool AreWorkersIdle() const;

private:
	std::optional<Job> Lock_PopJob();

	friend class JobWorker;
};
} // namespace Core
