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

bool ArchiveReader::Load(std::string_view archivePath)
{
	Chk();
	return PHYSFS_mount(archivePath.data(), nullptr, 1) != 0;
}

bool ArchiveReader::IsPresent(std::string_view pathInArchive) const
{
	Chk();
	return PHYSFS_exists(pathInArchive.data()) != 0;
}

std::vector<u8> ArchiveReader::Decompress(std::string_view pathInArchive) const
{
	Chk();
	if (IsPresent(pathInArchive))
	{
		PHYSFS_File* pFile = PHYSFS_openRead(pathInArchive.data());
		PHYSFS_sint64 size = PHYSFS_fileLength(pFile);
		if (size > 0)
		{
			std::vector<u8> buffer(static_cast<size_t>(size), 0);
			PHYSFS_readBytes(pFile, buffer.data(), static_cast<PHYSFS_uint64>(buffer.size()));
			return buffer;
		}
	}
	return {};
}
} // namespace Core
