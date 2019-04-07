#pragma once
#include <mutex>
#include <functional>
#include <future>
#include "CoreTypes.h"
#include "SimpleTime.h"
#include "LittleEngine/Services/IService.h"

namespace LittleEngine
{
class JobManager final : public IService
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
		SPtr<class JobHandle> m_sHandle;
		std::function<void()> m_task;
		s32 m_id;
		bool m_bSilent = false;

	public:
		Job() = default;
		Job(s32 id, std::function<void()> task, String name, bool bSilent);

		const char* ToStr() const;

		void Run();
		void Fulfil();
	};

private:
	Vec<UPtr<class JobWorker>> m_gameWorkers;
	Vec<UPtr<class JobWorker>> m_engineWorkers;
	List<UPtr<class MultiJob>> m_uMultiJobs;
	List<UPtr<Job>> m_gameJobQueue;
	List<UPtr<Job>> m_engineJobQueue;
	std::mutex m_mutex;
	s64 m_nextGameJobID = 100;
	s64 m_nextEngineJobID = 0;
	u8 m_availableEngineThreads;

public:
	JobManager();
	~JobManager() override;

public:
	SPtr<JobHandle> Enqueue(const std::function<void()>& Task, String name = "", bool bSilent = false);
	SPtr<JobHandle> EnqueueEngine(const std::function<void()>& Task, String name);
	MultiJob* CreateMultiJob(String name);

private:
	s32 AvailableEngineThreads() const;
	void Tick(Time dt);

	UPtr<Job> Lock_PopJob(bool bEngineQueue = false);
	SPtr<JobHandle> Lock_Enqueue(UPtr<Job>&& uJob, List<UPtr<Job>>& jobQueue);

	friend class EngineService;
	friend class EngineLoop;
	friend class JobWorker;
};
} // namespace LittleEngine
