/*
 * == LittleEngine SysDialog ==
 *   Copyright 2019 Karn Kaul
 * Features:
 *   - MessageBox / X11 implementations as a system dialogue box
 *   - Supports three responses (as MessageBox buttons / X11 keypresses)
 * Usage:
 *   - Prepare `DialogueData` and call `CreateSystemDialogue(std::move(data))`
 */

#pragma once
#include <functional>
#include <string>

namespace LE
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
} // namespace LE
