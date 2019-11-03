#pragma once
#include <functional>
#include <string>
#include <vector>
#include "GameTypes/StdTypes.h"

namespace Core
{
// \brief File persistor; implementation uses fstream
class FileRW
{
private:
	std::string m_path;
	std::vector<std::string> m_lines;

public:
	FileRW(std::string path);

	bool Exists() const;
	std::string Load(bool bDiscardNewLines = true);
	bool Write(std::string contents, bool bAppend = false);
	bool Append(std::string contents);

	const std::vector<std::string>& Lines() const;

private:
	void Read(std::function<void(std::string line)> procedure);
};
} // namespace Core
