#pragma once
#include "StdTypes.h"
#include <functional>

namespace Core
{
// \brief File persistor; implementation uses fstream
class FileRW
{
private:
	String m_path;
	Vec<String> m_lines;

public:
	FileRW(String path);

	bool Exists() const;
	String ReadAll(bool bDiscardNewLines = true);
	const Vec<String>& ReadLines();
	bool Write(String contents, bool bAppend = false);
	bool Append(String contents);

private:
	void Read(std::function<void(String line)> procedure);
};
} // namespace Core
