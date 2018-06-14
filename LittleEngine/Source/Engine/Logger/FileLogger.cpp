#include "stdafx.h"
#include "FileLogger.h"
#include <iostream>
#include <chrono>
#include "Utils/FileRW.h"

namespace Game {
	FileLogger::FileLogger(const std::string & logFilePath, bool clearFile) {
		file = std::make_unique<FileRW>(logFilePath);
		if (clearFile) file->Write("");
		std::cout << "FileLogger created and writing to [ " << logFilePath << "] (console only)" << std::endl;
		worker = std::move(Spawn());
	}
	
	FileLogger::~FileLogger() {
		std::string flush;
		for (const auto& str : buffer) {
			flush += str;
		}
		file->Append(flush);
		buffer.clear();
		writing = false;
		worker.join();
		std::cout << "FileLogger destroyed (console only)" << std::endl;		
	}

	void FileLogger::AddToBuffer(const std::string & message) {
		std::lock_guard<std::mutex> lock(mutex);
		std::string newLine = message + "\n";
		buffer.push_back(newLine);
	}

	std::thread FileLogger::Spawn() {
		return std::thread(
			[this] { AsyncWrite(); }
		);
	}

	void FileLogger::AsyncWrite() {
		while (writing) {
			if (!pauseWriting) {
				std::lock_guard<std::mutex> lock(mutex);
				std::string flush;
				for (const auto& str : buffer) {
					flush += str;
				}
				file->Append(flush);
				buffer.clear();
			}
			int ms = 0;
			while (ms++ < 1000) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if (!writing) {
					break;
				}
			}
		}
	}
}
