#include "le_stdafx.h"
#include "FileLogger.h"
#include <iostream>
#include <chrono>

namespace LittleEngine {
	using Lock = std::lock_guard<std::mutex>;
	namespace Logger {
		extern void Cout(const std::string& log);
	}

	FileLogger::FileLogger(const std::string & logFilePath, bool bClearFile) {
		m_file = std::make_unique<GameUtils::FileRW>(logFilePath);
		if (bClearFile) m_file->Write("");
		Logger::Cout("FileLogger created and writing to [" + logFilePath + "] (console only)");
		// std::thread does not work directly with class methods, so using a lambda that calls it
		m_worker = std::thread(
			[this] { AsyncWrite(); }
		);
	}
	
	FileLogger::~FileLogger() {
		std::string flush = FlushBuffer();
		m_file->Append(flush);
		m_bWriting = false;
		m_worker.join();
		Logger::Cout("FileLogger destroyed (console only)");
	}

	void FileLogger::AddToBuffer(std::string&& message) {
		Lock lock(m_mutex);
		m_buffer += (message + "\n");
	}

	void FileLogger::AsyncWrite() {
		while (m_bWriting) {
			if (!m_bPauseWriting) {
				std::string flush = FlushBuffer();
				m_file->Append(flush);
			}
			for (int ms = 0; ms++ < 1000 && m_bWriting; ++ms) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	std::string FileLogger::FlushBuffer()
	{
		std::string flush;
		{
			Lock lock(m_mutex);
			flush = std::move(m_buffer);
		}
		return flush;
	}
}
