#include "DebugProfiler.h"
#if ENABLED(PROFILER)
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "Engine/Debug/Profiler.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Context/LEContext.h"
#include "Engine/GFX.h"
#include "Model/World/WorldClock.h"
#include "Model/GameManager.h"
#include "Framework/UI/Elements/UIProgressBar.h"

namespace LittleEngine::Debug::Profiler
{
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
u16 maxEntries = 30;
u16 killEntryAfterTimeScale = 10;
UByte barAlpha = 180;
bool bProfilerEnabled = false;

class Renderer
{
	UPtr<UIElement> m_uLabelRoot = nullptr;
	UPtr<UIElement> m_uBarRoot = nullptr;
	std::vector<UIEntry> m_uiEntries;
	u16 m_frameEntryCount = 0;

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
	auto top_1 = static_cast<LayerID>(ToS32(LayerID::Top) - 1);
	m_uLabelRoot = MakeUnique<UIElement>(top_1, true);
	m_uLabelRoot->OnCreate(*pContext, "ProfilerLabels");
	m_uLabelRoot->SetRectSize({textWidth, profilerHeight}, true);
	m_uLabelRoot->m_transform.nPosition = {-1, Fixed(-0.75f)};
	// m_uLabelRoot->SetPanel(Colour(100, 100, 100, 100));
	m_uBarRoot = MakeUnique<UIElement>(top_1, true);
	m_uBarRoot->OnCreate(*pContext, "ProfilerBars");
	m_uBarRoot->SetRectSize(Vector2(progressBarSize.x, profilerHeight), true);
	m_uBarRoot->m_transform.nPosition = {Fixed(0.8f), Fixed(-0.75f)};
	m_uBarRoot->Tick();
	m_uLabelRoot->Tick();
	// m_uBarRoot->SetPanel(Colour(100, 100, 100, 100));
	Colour colour = Colour::Yellow;
	for (u32 i = 0; i < maxEntries; ++i)
	{
		UIEntry newEntry;
		newEntry.uProgressBar = MakeUnique<UIProgressBar>(LayerID::Top, true);
		newEntry.uLabelElement = MakeUnique<UIElement>(LayerID::Top, true);
		newEntry.uProgressBar->OnCreate(*pContext, "", &m_uBarRoot->m_transform);
		newEntry.uLabelElement->OnCreate(*pContext, "", &m_uLabelRoot->m_transform);
		colour.a = barAlpha;
		newEntry.uProgressBar->InitProgressBar(progressBarSize, colour);
		newEntry.uProgressBar->m_transform.anchor = {-1, 0};
		newEntry.uProgressBar->Rect()->SetEnabled(bProfilerEnabled)->SetStatic(false);
		newEntry.uLabelElement->m_transform.anchor = {1, 0};
		newEntry.uLabelElement->Text()->SetEnabled(bProfilerEnabled);
		m_uiEntries.emplace_back(std::move(newEntry));
	}
	SetupPositions();
}

void Renderer::SetEnabled(bool bEnabled)
{
	m_uLabelRoot->Rect()->SetEnabled(bEnabled);
	m_uBarRoot->Rect()->SetEnabled(bEnabled);
	for (auto& entry : m_uiEntries)
	{
		entry.uLabelElement->Text()->SetEnabled(bEnabled);
		entry.uProgressBar->Rect()->SetEnabled(bEnabled);
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
			if (idx < maxEntries)
			{
				UIEntry& uiEntry = m_uiEntries[idx];
				uiEntry.uLabelElement->SetText(UIText(entry.id, 20, entry.colour));
				uiEntry.uProgressBar->SetProgress(entry.timeRatio);
				Colour barColour = entry.colour;
				barColour.a = barAlpha;
				uiEntry.uProgressBar->Rect()->SetColour(barColour);
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
	u16 count = 0;
	u16 max = maxEntries;
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

void Init(Time maxTickTime)
{
	Assert(g_pGameManager && g_pGFX, "GameManager/GFX is null!");
	pContext = g_pGameManager->Context();
	maxTickDeltaTime = Time::Milliseconds(10);
	maxTickDeltaTime = maxTickTime;
	textWidth = 300;
	Vector2 uiSpace = g_pGFX->UISpace();
	profilerHeight = uiSpace.y * Fixed(18, 30);
	Fixed vpWidth = uiSpace.x;
	progressBarSize = Vector2(vpWidth - textWidth - 50, 10);
	uRenderer = MakeUnique<Renderer>();
	Toggle(false);

#if ENABLED(TWEAKABLES)
	profiler.BindCallback([](std::string_view toggle) { Toggle(Strings::ToBool(std::string(toggle))); });
	pflr_maxDT.Set(Strings::ToString(maxTickDeltaTime.AsMilliseconds()));
	pflr_maxDT.BindCallback([](std::string_view max) { maxTickDeltaTime = Time::Milliseconds(Strings::ToS32(std::string(max))); });
#endif
}

void Toggle()
{
	Toggle(!bProfilerEnabled);
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
} // namespace LittleEngine::Debug::Profiler
#endif
