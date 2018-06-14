#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>

class FileRW;

namespace Game {
	class FileLogger {
	public:
		FileLogger(const std::string& fileName, bool clearFile = true);
		~FileLogger();
		void AddToBuffer(const std::string& message);
		// To pause writing (will not terminate/pause thread)
		bool pauseWriting = false;
	private:
		std::vector<std::string> buffer;
		std::unique_ptr<FileRW> file;
		std::mutex mutex;
		bool writing = true;

		FileLogger(const FileLogger&) = delete;
		FileLogger& operator=(const FileLogger&) = delete;
		std::thread worker;
		void AsyncWrite();
	};
}
