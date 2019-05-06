#include "stdafx.h"
#include <thread>
#include <ctime>
#include "FileRW.h"
#include "Logger.h"
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

AsyncFileLogger::AsyncFileLogger(String path) : m_filePath(std::move(path))
{
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
	m_uWriter = MakeUnique<FileRW>(m_filePath);
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
} // namespace LittleEngine
