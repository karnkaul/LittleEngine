#pragma once
#include "StdTypes.h"

namespace Core
{
class ArchiveReader final
{
public:
	ArchiveReader();
	~ArchiveReader();

	void Load(const char* szArchivePath);
	void Load(InitList<const char*> archivePaths);
	void Load(const Vec<String>& archivePaths);

	bool IsPresent(const char* szPathInArchive) const;
	Vec<u8> Decompress(const String& pathInArchive) const;
};
}