#pragma once
#include <atomic>
#include <mutex>
#include "CoreTypes.h"
#include "OS.h"

namespace Core
{
class FileLogger final
{
private:
	const String m_extension = ".log";
	String m_filename;
	String m_cache;
	UPtr<class FileRW> m_uWriter;
	std::atomic<bool> m_bStopLogging;
	std::mutex m_mutex;
	OS::Threads::Handle m_threadHandle;

public:
	FileLogger(String filename, u8 backupCount, String header = "");
	~FileLogger();

private:
	void Async_StartLogging();
	void RenameOldFiles(u16 countToKeep);
};
} // namespace LittleEngine
