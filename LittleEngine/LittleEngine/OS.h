#pragma once
#include "Core/CoreTypes.h"
#include "Core/OS.h"

namespace OS
{
struct Screen
{
	u32 width;
	u32 height;

	static Screen* Instance();
};
}