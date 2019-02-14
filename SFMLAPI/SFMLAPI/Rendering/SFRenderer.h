#pragma once
#include <atomic>
#include "GFXBuffer.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
// \brief: Base Renderer that uses an atomic conditional bool to allow derived classes to be asynchronous
class SFRenderer
{
public:
	std::atomic<bool> m_bRendering;

protected:
	class SFWindow* m_pSFWindow;
	Time m_tickRate;

public:
	SFRenderer(SFWindow& sfWindow, Time tickRate);

	void Render(GFXBuffer& buffer);
	void Display();
};
} // namespace LittleEngine
