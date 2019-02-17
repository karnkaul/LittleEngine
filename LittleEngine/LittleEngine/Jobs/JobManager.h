#pragma once
#include <atomic>
#include <mutex>
#include "CoreTypes.h"
#include "JobWorker.h"
#include "LittleEngine/Services/IService.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
using JobID = s32;

class MultiJob
{
private:
	struct SubJob
	{
		String name;
		Function(void()) job;

		SubJob(const String& name, Function(void()) job);
	};

	String m_logName;
	Vector<SubJob> m_subJobs;
	List<JobID> m_pendingJobIDs;
	Function(void()) m_OnComplete = nullptr;
	bool m_bCompleted = false;

public:
	MultiJob(const String& name);

	void AddJob(Function(void()) job, const String& name = "");
	void StartJobs(Function(void()) OnComplete);
	Fixed GetProgress() const;

private:
	void Update();
	const char* LogNameStr() const;

	friend class JobManager;
};

class JobManager final : public IService
{
public:
	static constexpr JobID INVALID_ID = -1;

private:
	using Lock = std::lock_guard<std::mutex>;

	struct Job
	{
		String logName;
		Function(void()) task;
		JobID id = INVALID_ID;

		Job() = default;
		Job(JobID id, Function(void()) task, String name);

		const char* ToString() const;
	};

private:
	Vector<UPtr<class JobWorker>> m_userWorkers;
	Vector<UPtr<class JobWorker>> m_systemWorkers;
	Vector<JobID> m_completed;
	List<UPtr<MultiJob>> m_uMultiJobs;
	List<Job> m_userJobQueue;
	List<Job> m_systemJobQueue;
	std::mutex m_mutex;
	JobID m_nextUserID = 100;
	JobID m_nextSystemID = 0;
	u32 m_availableSystemThreads;

public:
	JobManager();
	~JobManager();

	void Wait(JobID id);
	bool IsRunning(JobID id);

public:
	JobID Enqueue(Function(void()) Task, const String& name = "");
	MultiJob* CreateMultiJob(const String& name);

private:
	s32 AvailableSystemThreads() const;
	JobID EnqueueSystem(Function(void()) Task, const String& name);
	bool IsCompleted(JobID id);
	bool IsEnqueued(JobID id);
	JobWorker* GetUserWorker(JobID id);
	JobWorker* GetSystemWorker(JobID id);
	void Tick(Time dt);

	friend class MultiJob;
	friend class AsyncRenderLoop;
	friend class AsyncFileLogger;
	friend class EngineService;
	friend class EngineLoop;
	friend class JobWorker;
};
} // namespace LittleEngine
