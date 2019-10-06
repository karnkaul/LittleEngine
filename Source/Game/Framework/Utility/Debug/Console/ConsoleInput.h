#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(CONSOLE)
#include "Core/Game/RingBuffer.h"
#include "Engine/Input/LEInput.h"
#include "Engine/Input/KeyboardInput.h"

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
	std::string m_query;
	Core::RingBuffer<std::string> m_queryCache;
	Token m_token;
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
