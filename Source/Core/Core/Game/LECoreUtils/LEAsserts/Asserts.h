/*
 * == LittleEngine Asserts ==
 *   Copyright 2019 Karn Kaul
 * Features:
 *   - Pre-assert MessageBox / X11 implementations that block the main thread and wait for user response
 *   - Responses include: "Assert", "Ignore ID" (turn off this assert), "Ignore All" (turn off all asserts)
 *   - Will break if debugger detected (MSVC / gdb)
 *   - Does nothing if `ASSERTS == 0` or not defined
 * Usage:
 *   - Define `ASSERTS 1` to enable
 *   - Include this file and call `Assert(predicate, message)`
 */

#pragma once

#if defined(ASSERTS) && ASSERTS
#define Assert(predicate, errorMessage) LE::AssertWithMessage(!!(predicate), #errorMessage, __FILE__, __LINE__)
#define AssertVar(predicate, szStr) LE::AssertWithMessage(!!(predicate), szStr, __FILE__, __LINE__)
#else
#define Assert(disabled, _disabled)
#define AssertVar(disabled, _disabled)
#endif

namespace LE
{
void AssertWithMessage(bool expr, const char* message, const char* fileName, long lineNumber);
}
