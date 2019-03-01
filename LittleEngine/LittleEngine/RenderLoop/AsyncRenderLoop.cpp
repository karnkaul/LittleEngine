#include "stdafx.h"
#include "AsyncRenderLoop.h"
#include "RenderHeap.h"
#include "LittleEngine/Services/Services.h"
#include "Asserts.h"
#include "Logger.h"
#include "SFMLAPI/Rendering/GFXBuffer.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
AsyncRenderLoop::AsyncRenderLoop(SFWindow& sfWindow, GFXBuffer& gfxBuffer, Time tickRate, bool bStartThread)
	: SFRenderer(sfWindow, tickRate), m_pBuffer(&gfxBuffer)
{
	if (bStartThread)
	{
		LOG_I("[AsyncRenderLoop] Deactivating GLWindow on this thread, starting render thread");
		m_pSFWindow->setActive(false);
		m_renderThreadJobID = Services::Jobs()->EnqueueEngine(
			std::bind(&AsyncRenderLoop::Run, this), "Async Render Loop");
	}
}

AsyncRenderLoop::~AsyncRenderLoop()
{
	m_bRendering.store(false, std::memory_order_relaxed);
	if (m_renderThreadJobID != JobManager::INVALID_ID)
		Services::Jobs()->Wait(m_renderThreadJobID);
}

void AsyncRenderLoop::Run()
{
	LOG_I("R[AsyncRenderLoop] Activating GLWindow on this thread");
	m_pSFWindow->setActive(true);
	m_pSFWindow->setVerticalSyncEnabled(true);
	while (m_bRendering.load(std::memory_order_relaxed))
	{
		Assert(m_pSFWindow, "[AsyncRenderLoop] SFML RenderWindow is nullptr!");

		Render(*m_pBuffer);
		Display();
	}
	LOG_I("R[AsyncRenderLoop] Deactivating GLWindow on this thread, terminating job");
	m_pSFWindow->setActive(false);
}
} // namespace LittleEngine
