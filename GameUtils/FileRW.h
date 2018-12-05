#pragma once
#include <string>
#include <vector>
#include <functional>

namespace GameUtils {
	// \brief File persistor; implementation uses fstream
	class FileRW {
	private:
		std::string m_path;
		std::vector<std::string> m_lines;

	public:
		FileRW(std::string path) : m_path(path) {}

		bool Exists() const;
		std::string ReadAll(bool bDiscardNewLines = true);
		const std::vector<std::string>& ReadLines();
		bool Write(const std::string& contents, bool bAppend = false);
		bool Append(const std::string& contents);

	private:
		void Read(std::function<void(std::string&& line)> Procedure);
	};
}
