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
	const String m_extension = ".log";
	String m_filename;
	String m_cache;
	UPtr<Core::FileRW> m_uWriter;
	std::atomic<bool> m_bStopLogging;
	std::mutex m_mutex;
	JobHandle m_sFileLogJobHandle;

public:
	AsyncFileLogger(String filename, u8 backupCount);
	~AsyncFileLogger();

private:
	void Async_StartLogging();
	void RenameOldFiles(u8 countToKeep);
};
} // namespace LittleEngine
