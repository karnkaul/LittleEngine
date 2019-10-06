#pragma once
#include <string_view>
#include <vector>
#include "LECoreUtils/StdTypes.h"

namespace Core
{
class ArchiveReader final
{
public:
	static char* s_argv0;

public:
	ArchiveReader();

	bool Load(std::string_view archivePath);

	bool IsPresent(std::string_view pathInArchive) const;
	std::vector<u8> Decompress(std::string_view pathInArchive) const;
};
} // namespace Core
