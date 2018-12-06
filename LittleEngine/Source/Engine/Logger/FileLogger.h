#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "FileRW.h"

namespace LittleEngine {
	// \brief RAII Asynchronous file logger; sleeps thread for a millisecond 1000 times
	class FileLogger final {
	private:
		std::string m_buffer;
		std::unique_ptr<GameUtils::FileRW> m_file;
		std::mutex m_mutex;
		std::thread m_worker;
		bool m_bWriting = true;
	public:
		bool m_bPauseWriting = false;		// To pause writing (will not terminate/pause thread)

	public:
		FileLogger(const std::string& fileName, bool bClearFile = true);
		~FileLogger();

		void AddToBuffer(std::string&& message);
		
	private:
		FileLogger(const FileLogger&) = delete;
		FileLogger& operator=(const FileLogger&) = delete;
		
		void AsyncWrite();
		std::string FlushBuffer();
	};
}
