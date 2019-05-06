#pragma once
#include <atomic>
#include <mutex>
#include "Core/CoreTypes.h"
#include "LittleEngine/Jobs/JobHandle.h"

namespace Core
{
class FileRW;
}

namespace LittleEngine
{
class AsyncFileLogger final
{
private:
	String m_filePath;
	String m_cache;
	UPtr<Core::FileRW> m_uWriter;
	std::atomic<bool> m_bStopLogging;
	std::mutex m_mutex;
	JobHandle m_sFileLogJobHandle;

public:
	AsyncFileLogger(String filePath);
	~AsyncFileLogger();

private:
	void Async_StartLogging();
};
} // namespace LittleEngine
