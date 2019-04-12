#pragma once
#include "CoreTypes.h"
#if ENABLED(CONSOLE)
#include "CircularList.hpp"
#include "LittleEngine/Input/EngineInput.h"
#include "LittleEngine/Input/KeyboardInput.h"

namespace LittleEngine
{
namespace Debug
{
class ConsoleInput final
{
public:
	static constexpr char CONSOLE_KEY = '`';

	KeyboardInput m_keyboard;
	String m_query;
	CircularList<String> queryCache;
	EngineInput::Token m_token;
	bool bCyclingQueries = false;

	ConsoleInput();

	bool OnInput(const EngineInput::Frame& frame);
	void Update();
	String GetConsoleLiveLine(bool bShowCursor) const;

private:
	void UpdateLiveLine(const EngineInput::Frame& frame);
};
} // namespace Debug
} // namespace LittleEngine
#endif
