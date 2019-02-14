#pragma once
#include "CoreTypes.h"
#include <functional>
#include <vector>

namespace Core
{
struct DialogueData
{
	String title;
	String content;
	std::function<void()> OnButton0 = nullptr;
	std::function<void()> OnButton1 = nullptr;
	std::function<void()> OnButton2 = nullptr;
};

void CreateSystemDialogue(const DialogueData& data);
} // namespace Core
