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
		std::ifstream file(path);
		if (!file.good()) {
			return std::string();
		}

		std::string line;
		std::string ret;
		while (std::getline(file, line)) {
			m_lines.emplace_back(line);
			ret += line;
			if (!discardNewLines) {
				ret += '\n';
			}
		}
		file.close();
		return ret;
	}

	const std::vector<std::string>& FileRW::ReadLines() {
		m_lines.clear();
		std::ifstream file(path);
		if (!file.good()) {
			return m_lines;
		}

		std::string line;
		while (std::getline(file, line)) {
			m_lines.emplace_back(line);
		}
		file.close();
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
}
