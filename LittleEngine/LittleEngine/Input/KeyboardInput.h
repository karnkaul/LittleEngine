#pragma once
#include "StdTypes.h"
#include "LittleEngine/Input/EngineInput.h"

namespace LittleEngine
{
struct LiveLine
{
	String liveString;

	LiveLine();

	void Append(String newInput);
	void Backspace();
	void Clear();
};

class KeyboardInput final
{
public:
	LiveLine m_liveLine;
	bool m_bClearOnEscape = true;

public:
	void Update(const EngineInput::Frame& frame);
	void Clear();
	String GetLiveString() const;
};
} // namespace LittleEngine
