#include "stdafx.h"
#include <thread>
#include <ctime>
#include "FileLogger.h"
#include "FileRW.h"
#include "Logger.h"
#include "OS.h"
#include "Utils.h"

namespace Core
{
namespace
{
String GetPrologue(String header)
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char buffer[256];
	ctime_s(buffer, 256, &now);
	String ret(buffer);
	ret += std::move(header);
	return ret + "\n";
}
} // namespace
using Lock = std::lock_guard<std::mutex>;

FileLogger::FileLogger(String filename, u8 backupCount, String header) : m_filename(std::move(filename))
{
	RenameOldFiles(backupCount);
	Core::g_OnLogStr = [&](const char* pText) {
		if (!m_bStopLogging.load(std::memory_order_relaxed))
		{
			Lock lock(m_mutex);
			m_cache += std::string(pText);
		}
	};
	m_uWriter = MakeUnique<FileRW>(m_filename + m_extension);
	m_uWriter->Write(GetPrologue(std::move(header)));
	m_bStopLogging.store(false, std::memory_order_relaxed);
	m_threadHandle = OS::Threads::Spawn([&]() { Async_StartLogging(); });
}

FileLogger::~FileLogger()
{
	LOG_I("Logging terminated");
	// Freeze m_cache and terminate thread
	m_bStopLogging.store(true);
	OS::Threads::Join(m_threadHandle);
	Core::g_OnLogStr = nullptr;
}

void FileLogger::Async_StartLogging()
{
	while (!m_bStopLogging.load(std::memory_order_relaxed))
	{
		String toWrite;
		{
			Lock lock(m_mutex);
			toWrite = std::move(m_cache);
		}
		m_uWriter->Append(toWrite);
		std::this_thread::yield();
	}

	// m_cache is now read-only from main thread, no lock required
	if (!m_cache.empty())
	{
		m_uWriter->Append(std::move(m_cache));
	}
	m_uWriter = nullptr;
}

void FileLogger::RenameOldFiles(u16 countToKeep)
{
	auto BackupPath = [&](u16 id) { return m_filename + "_bak_" + Strings::ToString(id) + m_extension; };
	
	// Make room for oldest backup
	String oldest = BackupPath(countToKeep);
	std::ifstream logFile(oldest);
	if (logFile.good())
	{
		std::remove(oldest.c_str());
	}
	--countToKeep;
	s32 success = 0;

	// Rename old backups
	while (countToKeep > 0)
	{
		String from = BackupPath(countToKeep);
		String to = BackupPath(countToKeep + 1);
		std::ifstream logFile(from);
		if (logFile.good())
		{
			if (std::ifstream(to))
			{
				std::remove(to.c_str());
			}
			logFile.close();
			success += std::rename(from.c_str(), to.c_str());
		}
		--countToKeep;
	}

	// Rename last log file
	String from = m_filename + m_extension;
	String to = BackupPath(1);
	if (std::ifstream(from))
	{
		success += std::rename(from.c_str(), to.c_str());
	}

	if (success != 0)
	{
		LOG_E("[AsyncFileLogger] Could not rename all old log files");
	}
}
} // namespace LittleEngine
