#include "stdafx.h"
#include "DebugProfiler.h"
#if ENABLED(PROFILER)
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "LittleEngine/Debug/Profiler.h"
#include "LittleEngine/Debug/Tweakable.h"
#include "LittleEngine/Context/LEContext.h"
#include "LEGame/Framework/UI/Elements/UIProgressBar.h"

namespace LittleEngine
{
namespace Debug
{
namespace Profiler
{
using Lock = std::lock_guard<std::mutex>;

namespace
{
LEContext* pContext = nullptr;

struct UIEntry
{
	UPtr<UIProgressBar> uProgressBar;
	UPtr<UIElement> uLabelElement;
};

Vector2 progressBarSize;
Fixed textWidth;
Fixed profilerHeight;
u32 maxEntries = 30;
u32 killEntryAfterTimeScale = 10;
UByte barAlpha = 180;
bool bProfilerEnabled = false;

class Renderer
{
	UPtr<UIElement> m_uLabelRoot = nullptr;
	UPtr<UIElement> m_uBarRoot = nullptr;
	Vec<UIEntry> m_uiEntries;
	u32 m_frameEntryCount = 0;

public:
	Renderer();

	void SetEnabled(bool bEnabled);
	void Tick(Time dt);
	void Clear();

private:
	void SetupPositions();
};

Renderer::Renderer()
{
	LayerID top_1 = static_cast<LayerID>(LAYER_TOP - 1);
	m_uLabelRoot = MakeUnique<UIElement>(top_1, true);
	m_uLabelRoot->OnCreate(*pContext, "ProfilerLabels");
	m_uLabelRoot->m_transform.size = {textWidth, profilerHeight};
	m_uLabelRoot->m_transform.bAutoPad = true;
	m_uLabelRoot->m_transform.nPosition = {-1, Fixed(-0.75f)};
	// m_uLabelRoot->SetPanel(Colour(100, 100, 100, 100));
	m_uBarRoot = MakeUnique<UIElement>(top_1, true);
	m_uBarRoot->OnCreate(*pContext, "ProfilerBars");
	m_uBarRoot->m_transform.size = Vector2(progressBarSize.x, profilerHeight);
	m_uBarRoot->m_transform.bAutoPad = true;
	m_uBarRoot->m_transform.nPosition = {Fixed(0.8f), Fixed(-0.75f)};
	m_uBarRoot->Tick();
	m_uLabelRoot->Tick();
	// m_uBarRoot->SetPanel(Colour(100, 100, 100, 100));
	Colour colour = Colour::Yellow;
	for (u32 i = 0; i < maxEntries; ++i)
	{
		UIEntry newEntry;
		newEntry.uProgressBar = MakeUnique<UIProgressBar>(LAYER_TOP, true);
		newEntry.uLabelElement = MakeUnique<UIElement>(LAYER_TOP, true);
		newEntry.uProgressBar->OnCreate(*pContext, "", &m_uBarRoot->m_transform);
		newEntry.uLabelElement->OnCreate(*pContext, "", &m_uLabelRoot->m_transform);
		colour.a = barAlpha;
		newEntry.uProgressBar->InitProgressBar(progressBarSize, colour);
		newEntry.uProgressBar->m_transform.anchor = {-1, 0};
		newEntry.uProgressBar->GetPrimitive()->SetEnabled(bProfilerEnabled)->SetStatic(false);
		newEntry.uLabelElement->m_transform.anchor = {1, 0};
		newEntry.uLabelElement->GetText()->SetEnabled(bProfilerEnabled);
		m_uiEntries.emplace_back(std::move(newEntry));
	}
	SetupPositions();
}

void Renderer::SetEnabled(bool bEnabled)
{
	m_uLabelRoot->GetPrimitive()->SetEnabled(bEnabled);
	m_uBarRoot->GetPrimitive()->SetEnabled(bEnabled);
	for (auto& entry : m_uiEntries)
	{
		entry.uLabelElement->GetText()->SetEnabled(bEnabled);
		entry.uProgressBar->GetPrimitive()->SetEnabled(bEnabled);
	}
}

void Renderer::Tick(Time dt)
{
	Time now = Time::Now();
	auto iter = entries.begin();
	while (iter != entries.end())
	{
		Entry& entry = iter->second;
		if ((now - entry.startTime) > entry.maxTime.Scaled(killEntryAfterTimeScale))
		{
			if (entry.bCustom)
			{
				--m_frameEntryCount;
			}
			iter = entries.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	size_t top = 0;
	size_t bottom = maxEntries;
	size_t idx = 0;
	{
		Lock lock(entriesMutex);
		for (auto& kvp : entries)
		{
			auto& entry = kvp.second;
			if (entry.bCustom)
			{
				idx = --bottom;
			}
			else
			{
				idx = top++;
			}
			if (idx >= 0 && idx < maxEntries)
			{
				UIEntry& uiEntry = m_uiEntries[idx];
				uiEntry.uLabelElement->SetText(UIText(entry.id, 20, entry.colour));
				uiEntry.uProgressBar->SetProgress(entry.timeRatio);
				Colour barColour = entry.colour;
				barColour.a = barAlpha;
				uiEntry.uProgressBar->GetPrimitive()->SetPrimaryColour(barColour);
				uiEntry.uProgressBar->Tick(dt);
			}
		}
	}

	for (idx = top; idx < bottom; ++idx)
	{
		UIEntry& uiEntry = m_uiEntries[idx];
		uiEntry.uLabelElement->SetText("");
		uiEntry.uProgressBar->SetProgress(Fixed::Zero);
		uiEntry.uProgressBar->Tick(dt);
	}
}

void Renderer::Clear()
{
	m_uiEntries.clear();
}

void Renderer::SetupPositions()
{
	u32 count = 0;
	u32 max = maxEntries;
	for (auto& entry : m_uiEntries)
	{
		Fixed nY = Fixed(count, max);
		nY = 1 - (2 * nY);
		++count;
		entry.uProgressBar->m_transform.nPosition = {-1, nY - Fixed(0.022f)};
		entry.uLabelElement->m_transform.nPosition = {1, nY};
		entry.uProgressBar->Tick();
		entry.uLabelElement->Tick();
	}
}

using URenderer = UPtr<Renderer>;

URenderer uRenderer = nullptr;
} // namespace

TweakBool(profiler, nullptr);
TweakF32(pflr_maxDT, nullptr);

void Init(LEContext& context, Time maxTickTime)
{
	pContext = &context;
	maxTickDeltaTime = Time::Milliseconds(10);
	maxTickDeltaTime = maxTickTime;
	textWidth = 300;
	Vector2 worldSize = pContext->GetViewSize();
	profilerHeight = worldSize.y * Fixed(18, 30);
	Fixed screenWidth = worldSize.x;
	progressBarSize = Vector2(screenWidth - textWidth - 50, 10);
	uRenderer = MakeUnique<Renderer>();
	Toggle(false);

#if ENABLED(TWEAKABLES)
	profiler.BindCallback([](const String& toggle) { Toggle(Strings::ToBool(toggle)); });
	pflr_maxDT.Set(Strings::ToString(maxTickDeltaTime.AsMilliseconds()));
	pflr_maxDT.BindCallback(
		[](const String& max) { maxTickDeltaTime = Time::Milliseconds(Strings::ToS32(max)); });
#endif
}

void Toggle(bool bEnable)
{
	bProfilerEnabled = bEnable;
	uRenderer->SetEnabled(bEnable);
}

void Cleanup()
{
	pContext = nullptr;
	if (uRenderer)
	{
		uRenderer->Clear();
	}
	uRenderer = nullptr;
}

void Tick(Time dt)
{
	if (uRenderer)
	{
		uRenderer->Tick(dt);
	}
}
} // namespace Profiler
} // namespace Debug
} // namespace LittleEngine
#endif
