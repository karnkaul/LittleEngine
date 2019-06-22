#pragma once
#include "Core/CoreTypes.h"
#include "JobHandle.h"

namespace Core
{
class JobManager final
{
public:
	static constexpr s32 INVALID_ID = -1;

private:
	class Job
	{
	private:
		std::promise<void> m_promise;
		String logName;

	public:
		JobHandle m_sHandle;
		Task m_task;
		s32 m_id;
		bool m_bSilent = false;
		const char* m_szException = nullptr;

	public:
		Job();
		Job(s32 id, Task task, String name, bool bSilent);

		const char* ToStr() const;

		void Run();
		void Fulfil();
	};

private:
	Vec<UPtr<class JobWorker>> m_jobWorkers;
	List<UPtr<class MultiJob>> m_uMultiJobs;
	List<UPtr<Job>> m_jobQueue;
	mutable std::mutex m_mutex;
	s64 m_nextGameJobID = 0;

public:
	JobManager(u32 workerCount);
	~JobManager();

public:
	JobHandle Enqueue(Task task, String name = "", bool bSilent = false);
	MultiJob* CreateMultiJob(String name);
	void ForEach(std::function<void(size_t)> indexedTask,
				 size_t iterationCount,
				 size_t iterationsPerJob,
				 size_t startIdx = 0);

	void Update();
	bool AreWorkersIdle() const;

private:
	UPtr<Job> Lock_PopJob();
	JobHandle Lock_Enqueue(UPtr<Job>&& uJob, List<UPtr<Job>>& jobQueue);

	friend class JobWorker;
};
} // namespace Core
