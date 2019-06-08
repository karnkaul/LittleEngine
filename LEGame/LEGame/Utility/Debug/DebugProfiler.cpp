#include "stdafx.h"
#include "DebugProfiler.h"
#if ENABLED(PROFILER)
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "LittleEngine/Debug/Tweakable.h"
#include "LittleEngine/Context/LEContext.h"
#include "LEGame/Framework/UI/Elements/UIProgressBar.h"

namespace LittleEngine
{
namespace Debug
{
namespace Profiler
{
#pragma region Local
namespace
{
LEContext* pContext = nullptr;

struct Entry
{
	UIProgressBar progressBar;
	UIElement labelElement;
	String id;
	Colour colour;
	Time startTime = Time::Zero;
	Time endTime = Time::Zero;
	Time maxTime = Time::Milliseconds(1);
	bool bFrameEntry;

	Entry(String id, Colour colour, Time startTime, Time maxTime, LayerID layer, bool bFrameEntry);
	Time Elapsed() const;
};

Entry::Entry(String id, Colour colour, Time startTime, Time maxTime, LayerID layer, bool bFrameEntry)
	: progressBar(UIProgressBar(layer, true)),
	  labelElement(UIElement(layer, true)),
	  id(std::move(id)),
	  colour(colour),
	  startTime(startTime),
	  maxTime(maxTime),
	  bFrameEntry(bFrameEntry)
{
}
Time Entry::Elapsed() const
{
	return endTime - startTime;
}

Vector2 progressBarSize;
Fixed textWidth;
Fixed profilerHeight;
Time maxTickDeltaTime;
u32 maxEntries = 30;
UByte globalAlpha = 200;

class Renderer
{
	UPtr<UIElement> m_uLabelRoot = nullptr;
	UPtr<UIElement> m_uBarRoot = nullptr;
	UMap<String, UPtr<Entry>> m_entries;
	u32 m_frameEntryCount = 0;

public:
	Renderer();

	void SetEnabled(bool bEnabled);
	void Tick(Time dt);
	void Start(String id, Colour colour, Time maxTime, bool bFramed, bool bEnabled);
	void Stop(String id);
	void Clear();

private:
	void SetupNewEntry(Entry& newEntry, Colour colour, bool bEnabled);
	void SetupPositions();
};

Renderer::Renderer()
{
	m_uLabelRoot = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uLabelRoot->OnCreate(*pContext, "ProfilerLabels");
	m_uLabelRoot->m_transform.size = {textWidth, profilerHeight};
	m_uLabelRoot->m_transform.bAutoPad = true;
	m_uLabelRoot->m_transform.nPosition = {-1, Fixed(-0.75f)};
	//m_uLabelRoot->SetPanel(Colour(100, 100, 100, 100));
	m_uBarRoot = MakeUnique<UIElement>(LAYER_TOP, true);
	m_uBarRoot->OnCreate(*pContext, "ProfilerBars");
	m_uBarRoot->m_transform.size = Vector2(progressBarSize.x, profilerHeight);
	m_uBarRoot->m_transform.bAutoPad = true;
	m_uBarRoot->m_transform.nPosition = {Fixed(0.9f), Fixed(-0.75f)};
	//m_uBarRoot->SetPanel(Colour(100, 100, 100, 100));
}

void Renderer::SetEnabled(bool bEnabled)
{
	m_uLabelRoot->GetPrimitive()->SetEnabled(bEnabled);
	m_uBarRoot->GetPrimitive()->SetEnabled(bEnabled);
	for (auto& entry : m_entries)
	{
		entry.second->labelElement.GetText()->SetEnabled(bEnabled);
		entry.second->progressBar.GetPrimitive()->SetEnabled(bEnabled);
	}
}

void Renderer::Tick(Time dt)
{
	m_uBarRoot->Tick(dt);
	m_uLabelRoot->Tick(dt);
	for (auto& entry : m_entries)
	{
		entry.second->labelElement.Tick(dt);
		entry.second->progressBar.Tick(dt);
	}

	Time now = Time::Now();
	auto iter = m_entries.begin();
	while (iter != m_entries.end())
	{
		UPtr<Entry>& uEntry = iter->second;
		Time maxTime = uEntry->maxTime;
		if ((now - uEntry->startTime) > maxTime.Scale(100))
		{
			if (uEntry->bFrameEntry)
			{
				--m_frameEntryCount;
			}
			iter = m_entries.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	SetupPositions();
}

void Renderer::Start(String id, Colour colour, Time maxTime, bool bFramed, bool bEnabled)
{
	auto search = m_entries.find(id);
	if (search != m_entries.end())
	{
		search->second->startTime = Time::Now();
	}
	else
	{
		UPtr<Entry> uNewEntry =
			MakeUnique<Entry>(id, colour, Time::Now(), maxTime, LAYER_TOP, bFramed);
		SetupNewEntry(*uNewEntry, colour, bEnabled);
		m_entries.emplace(std::move(id), std::move(uNewEntry));
	}
}

void Renderer::Stop(String id)
{
	auto search = m_entries.find(id);
	if (search != m_entries.end())
	{
		search->second->endTime = Time::Now();
		f32 progress = search->second->Elapsed().AsSeconds() / search->second->maxTime.AsSeconds();
		search->second->progressBar.SetProgress(Fixed(progress));
		search->second->progressBar.Tick(Time::Zero);
	}
}

void Renderer::Clear()
{
	m_entries.clear();
}

void Renderer::SetupNewEntry(Entry& newEntry, Colour colour, bool bEnabled)
{
	newEntry.progressBar.OnCreate(*pContext, "", &m_uBarRoot->m_transform);
	newEntry.labelElement.OnCreate(*pContext, "", &m_uLabelRoot->m_transform);
	colour.a = globalAlpha;
	newEntry.progressBar.InitProgressBar(progressBarSize, colour);
	newEntry.progressBar.m_transform.anchor = {-1, 0};
	newEntry.progressBar.GetPrimitive()->SetEnabled(bEnabled);
	newEntry.labelElement.SetText(UIText(newEntry.id, 20, newEntry.colour));
	newEntry.labelElement.m_transform.anchor = {1, 0};
	newEntry.labelElement.GetText()->SetEnabled(bEnabled);
}

void Renderer::SetupPositions()
{
	u32 top = 0;
	u32 bottom = 0;
	u32 max = maxEntries;
	Assert(m_entries.size() <= maxEntries, "Too many profiler entries!");
	for (auto& kvp : m_entries)
	{
		auto& entry = kvp.second;
		Fixed nY;
		if (entry->bFrameEntry)
		{
			nY = Fixed(bottom, max);
			nY = (2 * nY) - 1;
			++bottom;
		}
		else
		{
			nY = Fixed(top, max);
			nY = 1 - (2 * nY);
			++top;
		}
		entry->progressBar.m_transform.nPosition = {-1, nY - Fixed(0.022f)};
		entry->labelElement.m_transform.nPosition = {1, nY};
	}
}

using URenderer = UPtr<Renderer>;

URenderer uRenderer = nullptr;
bool bEnabled = false;
} // namespace
#pragma endregion

#pragma region Interface
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
	progressBarSize = Vector2(screenWidth - textWidth - 30, 10);
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
	bEnabled = bEnable;
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

void Reset()
{
	if (uRenderer)
	{
		uRenderer->Clear();
	}
}

void StartTicked(String id, Colour colour)
{
	if (uRenderer)
	{
		uRenderer->Start(std::move(id), colour, maxTickDeltaTime, false, bEnabled);
	}
}

void StartCustom(String id, Time maxTime, Colour colour)
{
	if (uRenderer)
	{
		uRenderer->Start(std::move(id), colour, maxTime, true, bEnabled);
	}
}

void Stop(String id)
{
	if (uRenderer)
	{
		uRenderer->Stop(std::move(id));
	}
}
#pragma endregion
} // namespace Profiler
} // namespace Debug
} // namespace LittleEngine
#endif
