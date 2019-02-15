#include "stdafx.h"
#include <mutex>
#include "Utils.h"
#include "Logger.h"
#include "SFRenderer.h"
#include "SFRenderState.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "SFMLAPI/SFSystem.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
SFRenderer::SFRenderer(SFWindow& sfWindow, Time tickRate)
	: m_bRendering(true), m_pSFWindow(&sfWindow), m_tickRate(tickRate)
{
}

void SFRenderer::Render(GFXBuffer& buffer)
{
	if (m_bRendering)
	{
		m_pSFWindow->clear();

		Fixed renderDT = Time::Now().AsMilliseconds() - buffer.GetLastSwapTime().AsMilliseconds();
		Fixed tickRate = m_tickRate.AsMilliseconds();
		Fixed alpha = Maths::Clamp01(renderDT / tickRate);
		buffer.Lock_Traverse([&](Vector<SFPrimitive>& active) {
			for (auto& primitive : active)
			{
				primitive.UpdateRenderState(alpha);
				m_pSFWindow->draw(primitive.m_circle);
				m_pSFWindow->draw(primitive.m_rectangle);
				m_pSFWindow->draw(primitive.m_sprite);
				m_pSFWindow->draw(primitive.m_text);
			}
		});
	}
}

void SFRenderer::Display()
{
	if (m_bRendering)
	{
		m_pSFWindow->display();
	}
}
} // namespace LittleEngine
