#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "Utils/FileRW.h"

namespace LittleEngine {
	// \brief RAII Asynchronous file logger; sleeps thread for a millisecond 1000 times
	class FileLogger {
	public:
		FileLogger(const std::string& fileName, bool clearFile = true);
		~FileLogger();
		void AddToBuffer(const std::string& message);
		// To pause writing (will not terminate/pause thread)
		bool pauseWriting = false;
	private:
		std::vector<std::string> buffer;
		std::unique_ptr<Utils::FileRW> file;
		std::mutex mutex;
		bool writing = true;

		FileLogger(const FileLogger&) = delete;
		FileLogger& operator=(const FileLogger&) = delete;
		std::thread worker;
		void AsyncWrite();
	};
}
