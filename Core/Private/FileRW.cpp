#include "stdafx.h"
#include "FileRW.h"
#include <fstream>
#include <iostream>

namespace Core
{
FileRW::FileRW(const String& path) : m_path(path)
{
}

bool FileRW::Exists() const
{
	std::ifstream f(m_path.c_str());
	return f.good();
}

String FileRW::ReadAll(bool bDiscardNewLines)
{
	m_lines.clear();
	String ret;
	Read([&](String&& line) {
		ret += line;
		if (!bDiscardNewLines)
		{
			ret += '\n';
		}
		m_lines.push_back(std::move(line));
	});
	return ret;
}

const Vector<String>& FileRW::ReadLines()
{
	m_lines.clear();
	Read([&](String&& line) { m_lines.push_back(std::move(line)); });
	return m_lines;
}

bool FileRW::Write(const String& contents, bool append)
{
	try
	{
		s32 mode = std::ofstream::out;
		mode |= (append) ? std::ofstream::app : std::ofstream::trunc;
		std::ofstream file(m_path, mode);
		file << contents;
		file.close();
		return true;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
}

bool FileRW::Append(const String& contents)
{
	return Write(contents, true);
}

void FileRW::Read(Function(void(String&& line)) Procedure)
{
	std::ifstream file(m_path);
	if (!file.good())
	{
		return;
	}

	String line;
	while (std::getline(file, line))
	{
		Procedure(std::move(line));
	}
	file.close();
}
} // namespace Core
