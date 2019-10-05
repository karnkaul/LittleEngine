#include <fstream>
#include "FileRW.h"
#include "Logger.h"

namespace Core
{
FileRW::FileRW(std::string path) : m_path(std::move(path)) {}

bool FileRW::Exists() const
{
	return std::ifstream(m_path.c_str()).good();
}

std::string FileRW::Load(bool bDiscardNewLines)
{
	m_lines.clear();
	std::string ret;
	Read([&](std::string&& line) {
		ret += line;
		if (!bDiscardNewLines)
		{
			ret += '\n';
		}
		m_lines.emplace_back(std::move(line));
	});
	return ret;
}

bool FileRW::Write(std::string contents, bool append)
{
	try
	{
		s32 mode = std::ofstream::out;
		mode |= (append) ? std::ofstream::app : std::ofstream::trunc;
		std::ofstream file(m_path, static_cast<std::ofstream::openmode>(mode));
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

bool FileRW::Append(std::string contents)
{
	return Write(std::move(contents), true);
}

const std::vector<std::string>& FileRW::Lines() const
{
	return m_lines;
}

void FileRW::Read(std::function<void(std::string line)> procedure)
{
	std::ifstream file(m_path.c_str());
	if (file.good())
	{
		std::string line;
		while (std::getline(file, line))
		{
			procedure(std::move(line));
		}
	}
}
} // namespace Core
