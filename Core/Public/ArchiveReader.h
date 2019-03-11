#pragma once
#include "StdTypes.h"

namespace Core
{
class ArchiveReader final
{
private:
	static bool s_bInit;

public:
	ArchiveReader();

	void Load(const char* szArchivePath);
	void Load(InitList<const char*> archivePaths);
	void Load(const Vec<String>& archivePaths);

	bool IsPresent(const char* szPathInArchive) const;
	Vec<u8> Decompress(const char* szPathInArchive) const;

public:
	static void UnInit();
	static String ToText(const Vec<u8>& rawBuffer);
};
} // namespace Core