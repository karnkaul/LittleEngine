#pragma once
#include <atomic>
#include <mutex>
#include <functional>
#include <future>
#include "CoreTypes.h"
#include "JobHandle.h"
#include "JobWorker.h"
#include "MultiJob.h"
#include "LittleEngine/Services/IService.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
class JobManager final : public IService
{
public:
	static constexpr s32 INVALID_ID = -1;

private:
	using Lock = std::lock_guard<std::mutex>;

	struct Job
	{
		String logName;
		std::function<void()> task;
		JobHandle id = INVALID_ID;
		bool bSilent = false;

		Job() = default;
		Job(JobHandle id, const std::function<void()>& task, String name, bool bSilent);

		const char* ToStr() const;
	};

private:
	Vec<UPtr<class JobWorker>> m_gameWorkers;
	Vec<UPtr<class JobWorker>> m_engineWorkers;
	Vec<JobHandle> m_completed;
	List<UPtr<MultiJob>> m_uMultiJobs;
	List<Job> m_gameJobQueue;
	List<Job> m_engineJobQueue;
	std::mutex m_mutex;
	s64 m_nextGameJobID = 100;
	s64 m_nextEngineJobID = 0;
	u8 m_availableEngineThreads;

public:
	JobManager();
	~JobManager();

	void Wait(JobHandle id);
	void Wait(InitList<JobHandle> ids);
	void Wait(const Vec<JobHandle>& ids);
	bool IsRunning(JobHandle id);
	bool IsCompleted(JobHandle id);

public:
	JobHandle Enqueue(const std::function<void()>& Task, const String& name = "", bool bSilent = false);
	JobHandle EnqueueEngine(const std::function<void()>& Task, const String& name);
	MultiJob* CreateMultiJob(const String& name);

private:
	s32 AvailableEngineThreads() const;
	void Tick(Time dt);

	bool Unsafe_IsCompleted(JobHandle id);
	bool Unsafe_IsEnqueued(JobHandle id);
	JobWorker* Unsafe_GetGameWorker(JobHandle id);
	JobWorker* Unsafe_GetEngineWorker(JobHandle id);

	Job Lock_PopJob(bool bEngineQueue = false);
	void Lock_CompleteJob(JobHandle id);
	JobHandle Lock_Enqueue(Job&& job, List<Job>& jobQueue);

	friend class EngineService;
	friend class EngineLoop;
	friend class JobWorker;
};
} // namespace LittleEngine
