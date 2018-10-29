#pragma once
#include <string>
#include <vector>
#include <functional>

namespace GameUtils {
	// \brief File persistor; implementation uses fstream
	class FileRW {
	public:
		FileRW(std::string path) : path(path) {}

		bool Exists() const;
		std::string ReadAll(bool discardNewLines = true);
		const std::vector<std::string>& ReadLines();
		bool Write(const std::string& contents, bool append = false);
		bool Append(const std::string& contents);

	private:
		std::string path;
		std::vector<std::string> m_lines;

		void Read(std::function<void(std::string&& line)> Procedure);
	};
}
