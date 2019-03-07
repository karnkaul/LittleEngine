#include "stdafx.h"
#include <thread>
#include <time.h>
#include "AsyncFileLogger.h"
#include "Logger.h"
#include "LittleEngine/Engine/EngineConfig.h"
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

AsyncFileLogger::AsyncFileLogger(const String& path) : m_filePath(path)
{
	Core::g_OnLogStr = std::bind(&AsyncFileLogger::OnLogStr, this, _1);
	m_bStopLogging.store(false, std::memory_order_relaxed);
	m_sFileLogJobHandle = Services::Jobs()->EnqueueEngine(
		std::bind(&AsyncFileLogger::Async_StartLogging, this), "AsyncFileLogger");
}

AsyncFileLogger::~AsyncFileLogger()
{
	// Freeze m_cache and terminate thread
	m_bStopLogging.store(true, std::memory_order_relaxed);
	if (m_sFileLogJobHandle)
		m_sFileLogJobHandle->Wait();
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

void AsyncFileLogger::OnLogStr(const char* pText)
{
	if (!m_bStopLogging.load(std::memory_order_relaxed))
	{
		Lock lock(m_mutex);
		m_cache += std::string(pText);
	}
}
} // namespace LittleEngine
