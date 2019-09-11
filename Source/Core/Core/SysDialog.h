#pragma once
#include "CoreTypes.h"

namespace Core
{
struct DialogueResp
{
	String text;
	Task callback;
};

struct DialogueData
{
	String title;
	String footer;
	String content;
	DialogueResp resp0;
	DialogueResp resp1;
	DialogueResp resp2;
};

void CreateSystemDialogue(DialogueData data);
} // namespace Core
