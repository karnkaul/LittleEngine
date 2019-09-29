#pragma once
#include "StdTypes.h"

namespace Core
{
class ArchiveReader final
{
public:
	static char* s_argv0;

public:
	ArchiveReader();

	bool Load(VString archivePath);

	bool IsPresent(VString pathInArchive) const;
	Vec<u8> Decompress(VString pathInArchive) const;
};
} // namespace Core
