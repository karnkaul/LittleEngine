#include "stdafx.h"
#include "SysDialog.h"
#if _WIN32
#include <Windows.h>
#endif

namespace Core
{
namespace
{
#if _WIN32
s32 LaunchMessageBox(const DialogueData& data)
{
	UINT type = MB_CANCELTRYCONTINUE;

	s32 id = MessageBoxA(nullptr, data.content.c_str(), data.title.c_str(), type);

	switch (id)
	{
	case IDCANCEL:
		if (data.OnButton0)
		{
			data.OnButton0();
		}
		break;

	case IDTRYAGAIN:
		if (data.OnButton1)
		{
			data.OnButton1();
		}
		break;

	case IDCONTINUE:
		if (data.OnButton2)
		{
			data.OnButton2();
		}
		break;
	}
	return id;
}
#endif
} // namespace

void CreateSystemDialogue(const DialogueData& data)
{
#if _WIN32
	LaunchMessageBox(data);
#endif
}
} // namespace Core
