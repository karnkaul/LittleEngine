#pragma once
#include "StdTypes.h"

namespace Core
{
// \brief File persistor; implementation uses fstream
class FileRW
{
private:
	String m_path;
	Vector<String> m_lines;

public:
	FileRW(const String& path);

	bool Exists() const;
	String ReadAll(bool bDiscardNewLines = true);
	const Vector<String>& ReadLines();
	bool Write(const String& contents, bool bAppend = false);
	bool Append(const String& contents);

private:
	void Read(Function(void(String&& line)) Procedure);
};
} // namespace Core
