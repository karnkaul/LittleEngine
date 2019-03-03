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
	FileRW(const String& path);

	bool Exists() const;
	String ReadAll(bool bDiscardNewLines = true);
	const Vec<String>& ReadLines();
	bool Write(const String& contents, bool bAppend = false);
	bool Append(const String& contents);

private:
	void Read(const std::function<void(String&& line)>& Procedure);
};
} // namespace Core
