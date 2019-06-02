#pragma once
#include "CoreTypes.h"
#include <functional>

namespace Core
{
struct DialogueData
{
	String title;
	String content;
	Task OnButton0 = nullptr;
	Task OnButton1 = nullptr;
	Task OnButton2 = nullptr;
};

void CreateSystemDialogue(const DialogueData& data);
} // namespace Core
