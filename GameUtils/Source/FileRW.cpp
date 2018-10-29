#include <fstream>
#include <iostream>
#include "FileRW.h"

namespace GameUtils {
	bool FileRW::Exists() const {
		std::ifstream f(path.c_str());
		return f.good();
	}

	std::string FileRW::ReadAll(bool discardNewLines) {
		m_lines.clear();
		std::string ret;
		Read(
			[&](std::string&& line) {
				ret += line;
				if (!discardNewLines) {
					ret += '\n';
				}
				m_lines.push_back(std::move(line));
			}
		);
		return ret;
	}

	const std::vector<std::string>& FileRW::ReadLines() {
		m_lines.clear();
		Read(
			[&](std::string&& line) {
				m_lines.push_back(std::move(line));
			}
		);
		return m_lines;
	}

	bool FileRW::Write(const std::string& contents, bool append) {
		try {
			int mode = std::ofstream::out;
			mode |= (append) ? std::ofstream::app : std::ofstream::trunc;
			std::ofstream file(path, mode);
			file << contents;
			file.close();
			return true;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return false;
		}
	}

	bool FileRW::Append(const std::string & contents) {
		return Write(contents, true);
	}

	void FileRW::Read(std::function<void(std::string&& line)> Procedure) {
		std::ifstream file(path);
		if (!file.good()) {
			return;
		}

		std::string line;
		while (std::getline(file, line)) {
			Procedure(std::move(line));
		}
		file.close();
	}
}
