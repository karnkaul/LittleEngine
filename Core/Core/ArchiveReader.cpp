#include "stdafx.h"
#include "Asserts.h"
#include "ArchiveReader.h"
#include "PhysicsFS/physfs.h"

namespace Core
{
char* ArchiveReader::s_argv0 = nullptr;

namespace
{
struct PhysFSState
{
	bool bOk = false;

	PhysFSState();
	~PhysFSState();
};

PhysFSState::PhysFSState()
{
	s32 success = PHYSFS_init(ArchiveReader::s_argv0);
	bOk = (success == 1);
}

PhysFSState::~PhysFSState()
{
	PHYSFS_deinit();
}
} // namespace

static PhysFSState* s_pState = nullptr;

ArchiveReader::ArchiveReader()
{
	if (!s_pState)
	{
		static PhysFSState physfsState;
		s_pState = &physfsState;
	}
	Assert(s_pState && s_pState->bOk, "PhysFS not initialised!");
}

void ArchiveReader::Load(const char* szArchivePath)
{
	Assert(s_pState && s_pState->bOk, "PhysFS not initialised!");
	PHYSFS_addToSearchPath(szArchivePath, 1);
}

void ArchiveReader::Load(const Vec<String>& archivePaths)
{
	Assert(s_pState && s_pState->bOk, "PhysFS not initialised!");
	for (const auto& archivePath : archivePaths)
	{
		PHYSFS_addToSearchPath(archivePath.c_str(), 1);
	}
}

void ArchiveReader::Load(InitList<const char*> archivePaths)
{
	Assert(s_pState && s_pState->bOk, "PhysFS not initialised!");
	for (const char* szArchivePath : archivePaths)
	{
		PHYSFS_addToSearchPath(szArchivePath, 1);
	}
}

bool ArchiveReader::IsPresent(const char* szPathInArchive) const
{
	Assert(s_pState && s_pState->bOk, "PhysFS not initialised!");
	return PHYSFS_exists(szPathInArchive) != 0;
}

Vec<u8> ArchiveReader::Decompress(const char* szPathInArchive) const
{
	Assert(s_pState && s_pState->bOk, "PhysFS not initialised!");
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