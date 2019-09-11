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

static PhysFSState* s_pState = nullptr;

inline void Chk()
{
	Assert(s_pState && s_pState->bOk, "PhysFS not initialised!");
}
} // namespace

ArchiveReader::ArchiveReader()
{
	if (!s_pState)
	{
		static PhysFSState physfsState;
		s_pState = &physfsState;
	}
	Chk();
}

bool ArchiveReader::Load(const char* szArchivePath)
{
	Chk();
	return PHYSFS_mount(szArchivePath, nullptr, 1) != 0;
}

void ArchiveReader::Load(const Vec<String>& archivePaths)
{
	Chk();
	for (const auto& archivePath : archivePaths)
	{
		Load(archivePath.c_str());
	}
}

void ArchiveReader::Load(InitList<const char*> archivePaths)
{
	Chk();
	for (const char* szArchivePath : archivePaths)
	{
		Load(szArchivePath);
	}
}

bool ArchiveReader::IsPresent(const char* szPathInArchive) const
{
	Chk();
	return PHYSFS_exists(szPathInArchive) != 0;
}

Vec<u8> ArchiveReader::Decompress(const char* szPathInArchive) const
{
	Chk();
	if (IsPresent(szPathInArchive))
	{
		PHYSFS_File* pFile = PHYSFS_openRead(szPathInArchive);
		PHYSFS_sint64 size = PHYSFS_fileLength(pFile);
		if (size > 0)
		{
			Vec<u8> buffer(static_cast<size_t>(size), 0);
			PHYSFS_readBytes(pFile, buffer.data(), static_cast<PHYSFS_uint64>(buffer.size()));
			return buffer;
		}
	}
	return {};
}
} // namespace Core
