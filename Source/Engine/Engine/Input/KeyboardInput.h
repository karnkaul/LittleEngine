#pragma once
#include "Core/Game/LECoreUtils/StdTypes.h"
#include "Engine/Input/LEInput.h"

namespace LittleEngine
{
struct LiveLine
{
	std::string liveString;
	size_t cursorIdx;

	LiveLine();

	void Set(std::string newInput);
	void Append(std::string newInput);
	bool Backspace();
	bool Delete();
	void Clear();
	bool MoveCursor(s8 units);
	void MoveCursorToExtreme(bool bStart);

	Fixed CursorNPos() const;
};

class KeyboardInput final
{
public:
	LiveLine m_liveLine;
	bool m_bClearOnEscape = true;

public:
	void Update(const LEInput::Frame& frame);
	void Clear();
	std::string LiveString() const;
};
} // namespace LittleEngine
