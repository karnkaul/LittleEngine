#include "stdafx.h"
#include "FileRW.h"
#include "Logger.h"
#include <fstream>

namespace Core
{
FileRW::FileRW(String path) : m_path(std::move(path))
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
		m_lines.emplace_back(std::move(line));
	});
	return ret;
}

const Vec<String>& FileRW::ReadLines()
{
	m_lines.clear();
	Read([&](String&& line) { m_lines.emplace_back(std::move(line)); });
	return m_lines;
}

bool FileRW::Write(String contents, bool append)
{
	try
	{
		s32 mode = std::ofstream::out;
		mode |= (append) ? std::ofstream::app : std::ofstream::trunc;
		std::ofstream file(m_path, mode);
		file << std::move(contents);
		file.close();
		return true;
	}
	catch (const std::exception& e)
	{
		LOG_E("Error writing to file at [%s]!\n%s", m_path.c_str(), e.what());
		return false;
	}
}

bool FileRW::Append(String contents)
{
	return Write(std::move(contents), true);
}

void FileRW::Read(std::function<void(String line)> procedure)
{
	std::ifstream file(m_path);
	if (!file.good())
	{
		return;
	}

	String line;
	while (std::getline(file, line))
	{
		procedure(std::move(line));
	}
	file.close();
}
} // namespace Core
