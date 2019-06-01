#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(CONSOLE)
#include "Core/CircularVector.h"
#include "LittleEngine/Input/LEInput.h"
#include "LittleEngine/Input/KeyboardInput.h"

namespace LittleEngine
{
class LEContext;

namespace Debug
{
class ConsoleInput final
{
public:
	static constexpr char CONSOLE_KEY = '`';

public:
	KeyboardInput m_keyboard;
	String m_query;
	Core::CircularVector<String> m_queryCache;
	LEInput::Token m_token;
	bool bCyclingQueries = false;
private:
	LEContext* m_pContext;

public:
	ConsoleInput(LEContext& context);

	bool OnInput(const LEInput::Frame& frame);
	void Update();

private:
	void UpdateLiveLine(const LEInput::Frame& frame);
};
} // namespace Debug
} // namespace LittleEngine
#endif
