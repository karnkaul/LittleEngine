#pragma once
#include "CoreTypes.h"

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
	List<JobID> m_completedJobIDs;
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
} // namespace LittleEngine
