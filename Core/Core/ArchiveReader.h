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

	void Load(const char* szArchivePath);
	void Load(InitList<const char*> archivePaths);
	void Load(const Vec<String>& archivePaths);

	bool IsPresent(const char* szPathInArchive) const;
	Vec<u8> Decompress(const char* szPathInArchive) const;

public:
	static String ToText(Vec<u8> rawBuffer);
};
} // namespace Core