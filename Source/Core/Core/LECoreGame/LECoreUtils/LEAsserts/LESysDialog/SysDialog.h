#pragma once
#include <functional>
#include <string>

struct LEDialogueResp
{
	std::string text;
	std::function<void()> callback;
};

struct LEDialogueData
{
	std::string title;
	std::string footer;
	std::string content;
	LEDialogueResp resp0;
	LEDialogueResp resp1;
	LEDialogueResp resp2;
};

void LECreateSystemDialogue(LEDialogueData data);
