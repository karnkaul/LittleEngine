#include "le_stdafx.h"
#include "FileLogger.h"
#include <iostream>
#include <chrono>

namespace LittleEngine {
	FileLogger::FileLogger(const std::string & logFilePath, bool bClearFile) {
		m_file = std::make_unique<GameUtils::FileRW>(logFilePath);
		if (bClearFile) m_file->Write("");
		std::cout << "FileLogger created and writing to [ " << logFilePath << "] (console only)" << std::endl;
		// std::thread does not work directly with class methods, so using a lambda that calls it
		m_worker = std::thread(
			[this] { AsyncWrite(); }
		);
	}
	
	FileLogger::~FileLogger() {
		std::string flush;
		for (const auto& str : m_buffer) {
			flush += str;
		}
		m_file->Append(flush);
		m_buffer.clear();
		m_bWriting = false;
		m_worker.join();
		std::cout << "FileLogger destroyed (console only)" << std::endl;		
	}

	void FileLogger::AddToBuffer(const std::string & message) {
		std::lock_guard<std::mutex> lock(m_mutex);
		std::string newLine = message + "\n";
		m_buffer.push_back(newLine);
	}

	void FileLogger::AsyncWrite() {
		while (m_bWriting) {
			if (!m_bPauseWriting) {
				std::lock_guard<std::mutex> lock(m_mutex);
				std::string flush;
				for (const auto& str : m_buffer) {
					flush += str;
				}
				m_file->Append(std::move(flush));
				m_buffer.clear();
			}
			for (int ms = 0; ms++ < 1000 && m_bWriting; ++ms) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}
}
