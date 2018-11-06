#include "le_stdafx.h"
#include "FileLogger.h"
#include <iostream>
#include <chrono>

namespace LittleEngine {
	FileLogger::FileLogger(const std::string & logFilePath, bool bClearFile) {
		file = std::make_unique<GameUtils::FileRW>(logFilePath);
		if (bClearFile) file->Write("");
		std::cout << "FileLogger created and writing to [ " << logFilePath << "] (console only)" << std::endl;
		// std::thread does not work directly with class methods, so using a lambda that calls it
		worker = std::thread(
			[this] { AsyncWrite(); }
		);
	}
	
	FileLogger::~FileLogger() {
		std::string flush;
		for (const auto& str : buffer) {
			flush += str;
		}
		file->Append(flush);
		buffer.clear();
		bWriting = false;
		worker.join();
		std::cout << "FileLogger destroyed (console only)" << std::endl;		
	}

	void FileLogger::AddToBuffer(const std::string & message) {
		std::lock_guard<std::mutex> lock(mutex);
		std::string newLine = message + "\n";
		buffer.push_back(newLine);
	}

	void FileLogger::AsyncWrite() {
		while (bWriting) {
			if (!bPauseWriting) {
				std::lock_guard<std::mutex> lock(mutex);
				std::string flush;
				for (const auto& str : buffer) {
					flush += str;
				}
				file->Append(std::move(flush));
				buffer.clear();
			}
			for (int ms = 0; ms++ < 1000 && bWriting; ++ms) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}
}
