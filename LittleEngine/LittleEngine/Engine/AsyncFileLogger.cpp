#include "stdafx.h"
#include <thread>
#include "AsyncFileLogger.h"
#include "Logger.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
using Lock = std::lock_guard<std::mutex>;

AsyncFileLogger::AsyncFileLogger(const String& path) : m_filePath(path)
{
	Core::g_OnLogStr = std::bind(&AsyncFileLogger::OnLogStr, this, _1);
	m_bStopLogging.store(false);
	m_jobID = Services::Jobs()->EnqueueSystem(std::bind(&AsyncFileLogger::Async_StartLogging, this),
											  "AsyncFileLogger");
}

AsyncFileLogger::~AsyncFileLogger()
{
	Core::g_OnLogStr = nullptr;
	m_bStopLogging.store(true);
	Services::Jobs()->Wait(m_jobID);
	Assert(!m_uWriter, "Writer should be null!");
}

void AsyncFileLogger::Async_StartLogging()
{
	m_uWriter = MakeUnique<FileRW>(m_filePath);
	m_uWriter->Write("");
	while (!m_bStopLogging.load())
	{
		String toWrite;
		{
			Lock lock(m_mutex);
			toWrite = std::move(m_cache);
		}

		if (!toWrite.empty())
		{
			m_uWriter->Append(toWrite);
		}

		std::this_thread::yield();
	}
	m_uWriter->Append("[AsyncFileLogger] terminated\n");
	m_uWriter = nullptr;
}

void AsyncFileLogger::OnLogStr(const char* pText)
{
	Lock lock(m_mutex);
	m_cache += std::string(pText);
}
} // namespace LittleEngine
