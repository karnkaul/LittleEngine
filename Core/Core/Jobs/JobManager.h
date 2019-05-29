#pragma once
#include <mutex>
#include <functional>
#include <future>
#include "JobHandle.h"
#include "Core/CoreTypes.h"

namespace Core
{
class JobManager final
{
public:
	static constexpr s32 INVALID_ID = -1;

private:
	using Lock = std::lock_guard<std::mutex>;

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
	std::mutex m_mutex;
	s64 m_nextGameJobID = 0;

public:
	JobManager(u32 workerCount);
	~JobManager();

public:
	JobHandle Enqueue(Task task, String name = "", bool bSilent = false);
	MultiJob* CreateMultiJob(String name);
	void Update();
	bool AreWorkersIdle();

private:	
	UPtr<Job> Lock_PopJob();
	JobHandle Lock_Enqueue(UPtr<Job>&& uJob, List<UPtr<Job>>& jobQueue);
	
	friend class JobWorker;
};
} // namespace LittleEngine
