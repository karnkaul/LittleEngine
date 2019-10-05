#pragma once
#include "CoreTypes.h"

namespace Core
{
struct DialogueResp
{
	std::string text;
	std::function<void()> callback;
};

struct DialogueData
{
	std::string title;
	std::string footer;
	std::string content;
	DialogueResp resp0;
	DialogueResp resp1;
	DialogueResp resp2;
};

void CreateSystemDialogue(DialogueData data);
} // namespace Core
