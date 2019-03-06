#pragma once
#include <atomic>
#include <mutex>
#include "FileRW.h"
#include "LittleEngine/Jobs/JobManager.h"

namespace LittleEngine
{
class AsyncFileLogger final
{
private:
	String m_filePath;
	String m_cache;
	UPtr<FileRW> m_uWriter;
	std::atomic<bool> m_bStopLogging;
	std::mutex m_mutex;
	JobHandle m_jobHandle;

public:
	AsyncFileLogger(const String& filePath);
	~AsyncFileLogger();

private:
	void Async_StartLogging();
	void OnLogStr(const char* pText);
};
} // namespace LittleEngine
