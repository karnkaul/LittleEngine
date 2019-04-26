#include "stdafx.h"
#include "ArchiveReader.h"
#include "PhysicsFS/physfs.h"

namespace Core
{
bool ArchiveReader::s_bInit = false;

ArchiveReader::ArchiveReader()
{
	if (!s_bInit)
	{
		s32 success = PHYSFS_init(nullptr);
		if (success == 0)
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
	return PHYSFS_exists(szPathInArchive) != 0;
}

Vec<u8> ArchiveReader::Decompress(const char* szPathInArchive) const
{
	if (IsPresent(szPathInArchive))
	{
		PHYSFS_File* pFile = PHYSFS_openRead(szPathInArchive);
		PHYSFS_sint64 size = PHYSFS_fileLength(pFile);
		if (size > 0)
		{
			Vec<u8> buffer(size, 0);
			PHYSFS_readBytes(pFile, &buffer[0], size);
			return buffer;
		}
	}
	return {};
}

void ArchiveReader::UnInit()
{
	PHYSFS_deinit();
}

String ArchiveReader::ToText(Vec<u8> rawBuffer)
{
	Vec<char> charBuffer(rawBuffer.size() + 1, 0);
	for (size_t i = 0; i < rawBuffer.size(); ++i)
	{
		charBuffer[i] = static_cast<char>(rawBuffer[i]);
	}
	return String(charBuffer.data());
}
} // namespace Core