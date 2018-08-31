#pragma once
#include <string>
#include <vector>

// \brief File persistor; implementation uses fstream
class FileRW {
public:
	FileRW(std::string path) : path(path) {}

	bool Exists() const;
	const std::vector<std::string>& Read();
	bool Write(const std::string& contents, bool append = false);
	bool Append(const std::string& contents);

private:
	std::string path;
	std::vector<std::string> m_lines;
};
