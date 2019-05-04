#pragma once
#include "StdTypes.h"

#if ENABLED(ASSERTS)
#define Assert(predicate, errorMessage) \
	Core::AssertWithMsg(predicate, #errorMessage, __FILE__, __LINE__)
#define AssertVar(predicate, szStr) Core::AssertWithMsg(predicate, szStr, __FILE__, __LINE__)
#else
#define Assert(disabled, _disabled)
#define AssertVar(disabled, _disabled)
#endif

namespace Core
{
void AssertWithMsg(bool expr, const char* message, const char* fileName, long lineNumber);
}
