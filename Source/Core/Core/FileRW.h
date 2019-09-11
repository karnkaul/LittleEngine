#pragma once
#include <functional>
#include "StdTypes.h"

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
	String Load(bool bDiscardNewLines = true);
	bool Write(String contents, bool bAppend = false);
	bool Append(String contents);

	const Vec<String>& Lines() const;

private:
	void Read(std::function<void(String line)> procedure);
};
} // namespace Core
