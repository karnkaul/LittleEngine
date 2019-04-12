#include "stdafx.h"
#include "Asserts.h"
#include "Logger.h"
#include "SFMLAPI/Rendering/GFXBuffer.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "AsyncRenderLoop.h"
#include "RenderHeap.h"
#include "LittleEngine/Engine/OS.h"
#include "LittleEngine/Jobs/JobHandle.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
AsyncRenderLoop::AsyncRenderLoop(SFWindow& sfWindow, GFXBuffer& gfxBuffer, Time tickRate, bool bStartThread)
	: SFRenderer(sfWindow, tickRate), m_pBuffer(&gfxBuffer), m_bRunThread(bStartThread)
{
	Start();
}

AsyncRenderLoop::~AsyncRenderLoop()
{
	Stop();
}

void AsyncRenderLoop::RecreateWindow(SFWindowRecreateData data)
{
	Assert(OS::IsMainThread(), "AsyncRenderLoop::RecreateWindow() not called from Main thread!");
	Stop();
	m_pSFWindow->setActive(true);
	m_pSFWindow->DestroyWindow();
	m_pSFWindow->OverrideData(std::move(data));
	m_pSFWindow->CreateWindow();
	LOG_I("[AsyncRenderLoop] Activated SFWindow on this thread and recreated it");
	Start();
}

void AsyncRenderLoop::Start()
{
	if (m_bRunThread)
	{
		m_pSFWindow->setActive(false);
		m_bRendering.store(true, std::memory_order_relaxed);
		LOG_I("[AsyncRenderLoop] Deactivated SFWindow on this thread, starting render thread");
		m_pRenderJobHandle = Services::Jobs()->EnqueueEngine(
			std::bind(&AsyncRenderLoop::Async_Run, this), "Async Render Loop");
	}
}

void AsyncRenderLoop::Stop()
{
	m_bRendering.store(false, std::memory_order_relaxed);
	if (m_pRenderJobHandle)
	{
		m_pRenderJobHandle->Wait();
		m_pSFWindow->setActive(true);
	}
}

void AsyncRenderLoop::Async_Run()
{
	Assert(!OS::IsMainThread(), "AsyncRenderLoop::Async_Run() called from Main thread!");
	m_pSFWindow->setActive(true);
	m_pSFWindow->setVerticalSyncEnabled(true);
	LOG_I("R[AsyncRenderLoop] Activated SFWindow on this thread, starting render loop");
	while (m_bRendering.load(std::memory_order_relaxed))
	{
		Assert(m_pSFWindow, "[AsyncRenderLoop] SFML RenderWindow is nullptr!");
		Render(*m_pBuffer);
		Display();
	}
	m_pSFWindow->setActive(false);
	LOG_I("R[AsyncRenderLoop] Deactivated SFWindow on this thread, terminating render loop");
}
} // namespace LittleEngine
