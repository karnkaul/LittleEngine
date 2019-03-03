#include "stdafx.h"
#include "ArchiveReader.h"
#include "PhysicsFS/physfs.h"
#pragma comment(lib, "PhysicsFS.lib")

namespace Core
{
bool ArchiveReader::s_bInit = false;

ArchiveReader::ArchiveReader()
{
	if (!s_bInit)
	{
		s32 success = PHYSFS_init(nullptr);
		if (!success)
		{
			throw DependencyException();
		}
		s_bInit = true;
	}
}

void ArchiveReader::Load(const char* szArchivePath)
{
	PHYSFS_addToSearchPath(szArchivePath, 1);
}

void ArchiveReader::Load(const Vec<String>& archivePaths)
{
	for (const auto& archivePath : archivePaths)
	{
		PHYSFS_addToSearchPath(archivePath.c_str(), 1);
	}
}

void ArchiveReader::Load(InitList<const char*> archivePaths)
{
	for (const char* szArchivePath : archivePaths)
	{
		PHYSFS_addToSearchPath(szArchivePath, 1);
	}
}

bool ArchiveReader::IsPresent(const char* szPathInArchive) const
{
	return PHYSFS_exists(szPathInArchive);
}

Vec<u8> ArchiveReader::Decompress(const String& pathInArchive) const
{
	if (IsPresent(pathInArchive.c_str()))
	{
		PHYSFS_file* pFile = PHYSFS_openRead(pathInArchive.c_str());
		PHYSFS_sint64 size = PHYSFS_fileLength(pFile);
		Vec<u8> buffer(size, 0);
		PHYSFS_read(pFile, &buffer[0], 1, size);
		return buffer;
	}
	return {};
}

void ArchiveReader::UnInit()
{
	PHYSFS_deinit();
}

String ArchiveReader::ToText(const Vec<u8>& rawBuffer)
{
	Vec<char> charBuffer(rawBuffer.size() + 1, 0);
	for (size_t i = 0; i < rawBuffer.size(); ++i)
	{
		charBuffer[i] = static_cast<char>(rawBuffer[i]);
	}
	return String(charBuffer.data());
}
} // namespace Core