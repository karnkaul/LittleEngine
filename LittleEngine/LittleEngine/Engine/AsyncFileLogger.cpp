#include "stdafx.h"
#include <thread>
#include <ctime>
#include "Core/FileRW.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "AsyncFileLogger.h"
#include "LittleEngine/Engine/EngineConfig.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Jobs/JobHandle.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
namespace
{
String GetPrologue()
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char buffer[256];
	ctime_s(buffer, 256, &now);
	String ret(buffer);
	ret += "Game: " + GameManager::GetGameVersion().ToString();
	ret += " Engine: " + EngineConfig::GetEngineVersion().ToString();
	return ret + "\n";
}
} // namespace
using Lock = std::lock_guard<std::mutex>;

AsyncFileLogger::AsyncFileLogger(String filename, u8 backupCount) : m_filename(std::move(filename))
{
	RenameOldFiles(backupCount);
	Core::g_OnLogStr = [&](const char* pText) {
		if (!m_bStopLogging.load(std::memory_order_relaxed))
		{
			Lock lock(m_mutex);
			m_cache += std::string(pText);
		}
	};
	m_bStopLogging.store(false, std::memory_order_relaxed);
	m_sFileLogJobHandle =
		Services::Jobs()->EnqueueEngine([&]() { Async_StartLogging(); }, "AsyncFileLogger");
}

AsyncFileLogger::~AsyncFileLogger()
{
	// Freeze m_cache and terminate thread
	m_bStopLogging.store(true);
	if (m_sFileLogJobHandle)
	{
		m_sFileLogJobHandle->Wait();
	}
	Core::g_OnLogStr = nullptr;
}

void AsyncFileLogger::Async_StartLogging()
{
	m_uWriter = MakeUnique<FileRW>(m_filename + m_extension);
	m_uWriter->Write(GetPrologue());
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

void AsyncFileLogger::RenameOldFiles(u8 countToKeep)
{
	auto BackupPath = [&](u8 id) { return m_filename + "_bak_" + Strings::ToString(id) + m_extension; };
	
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
