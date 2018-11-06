#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "FileRW.h"

namespace LittleEngine {
	// \brief RAII Asynchronous file logger; sleeps thread for a millisecond 1000 times
	class FileLogger {
	public:
		FileLogger(const std::string& fileName, bool bClearFile = true);
		~FileLogger();
		void AddToBuffer(const std::string& message);
		// To pause writing (will not terminate/pause thread)
		bool bPauseWriting = false;
	private:
		std::vector<std::string> buffer;
		std::unique_ptr<GameUtils::FileRW> file;
		std::mutex mutex;
		bool bWriting = true;

		FileLogger(const FileLogger&) = delete;
		FileLogger& operator=(const FileLogger&) = delete;
		std::thread worker;
		void AsyncWrite();
	};
}
