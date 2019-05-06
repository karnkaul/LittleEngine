#include "stdafx.h"
#include "DebugProfiler.h"
#if ENABLED(PROFILER)
#include "Logger.h"
#include "Asserts.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "LittleEngine/Debug/Console/Tweakable.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIProgressBar.h"
#include "LittleEngine/UI/UIText.h"

namespace LittleEngine
{
extern Time g_maxFrameTime;

namespace Debug
{
namespace Profiler
{
#pragma region Local
namespace
{
std::thread::id safeThreadID;

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
Time maxFrameDeltaTime;
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
	void Start(String id, Colour colour, Time maxTime, bool bEnabled);
	void Stop(String id);
	void Clear();

private:
	void SetupNewEntry(Entry& newEntry, Colour colour, bool bEnabled);
	void SetupPositions();
};

Renderer::Renderer()
{
	m_uLabelRoot = MakeUnique<UIElement>("Profiler Labels Root", LAYER_TOP, true);
	m_uLabelRoot->m_transform.size = {textWidth, profilerHeight};
	m_uLabelRoot->m_transform.bAutoPad = true;
	m_uLabelRoot->m_transform.nPosition = {-1, Fixed(-0.80f)};
	// m_uLabelRoot->SetPanel(Colour(100, 100, 100, 100));
	m_uBarRoot = MakeUnique<UIElement>("Profiler Bars Root", LAYER_TOP, true);
	m_uBarRoot->m_transform.size = Vector2(progressBarSize.x, profilerHeight);
	m_uBarRoot->m_transform.bAutoPad = true;
	m_uBarRoot->m_transform.nPosition = {Fixed(0.8f), Fixed(-0.85f)};
	// m_uBarRoot->SetPanel(Colour(100, 100, 100, 100));
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
		if ((now - iter->second->startTime) > iter->second->maxTime.Scale(100))
		{
			if (iter->second->endTime == maxFrameDeltaTime)
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

void Renderer::Start(String id, Colour colour, Time maxTime, bool bEnabled)
{
	Assert(std::this_thread::get_id() == safeThreadID,
		   "Can only use Profiler on Engine Loop thread!");
	auto search = m_entries.find(id);
	if (search != m_entries.end())
	{
		search->second->startTime = Time::Now();
	}
	else
	{
		UPtr<Entry> uNewEntry =
			MakeUnique<Entry>(id, colour, Time::Now(), maxTime, LAYER_TOP, maxTime > maxTickDeltaTime);
		SetupNewEntry(*uNewEntry, colour, bEnabled);
		m_entries.emplace(std::move(id), std::move(uNewEntry));
	}
}

void Renderer::Stop(String id)
{
	Assert(std::this_thread::get_id() == safeThreadID,
		   "Can only use Profiler on Engine Loop thread!");
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
	colour.a = globalAlpha;
	newEntry.progressBar.SetParent(m_uBarRoot->m_transform);
	newEntry.progressBar.InitProgressBar(progressBarSize, colour);
	newEntry.progressBar.m_transform.anchor = {-1, 0};
	newEntry.progressBar.GetPrimitive()->SetEnabled(bEnabled);
	newEntry.labelElement.SetParent(m_uLabelRoot->m_transform);
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
		entry->maxTime = entry->bFrameEntry ? maxFrameDeltaTime : maxTickDeltaTime;
		entry->progressBar.m_transform.nPosition = {-1, nY};
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
TweakF32(pflr_maxFT, nullptr);

void Init(std::thread::id eventThreadID)
{
	safeThreadID = eventThreadID;
	maxTickDeltaTime = Time::Milliseconds(10);
	maxFrameDeltaTime = g_maxFrameTime;
	textWidth = 300;
	Vector2 worldSize = GFX::GetViewSize();
	profilerHeight = worldSize.y * Fixed(18, 30);
	Fixed screenWidth = worldSize.x;
	progressBarSize = Vector2(screenWidth - textWidth - 80, 10);
	uRenderer = MakeUnique<Renderer>();
	Toggle(false);

#if ENABLED(TWEAKABLES)
	profiler.BindCallback([](const String& toggle) { Toggle(Strings::ToBool(toggle)); });
	pflr_maxDT.Set(Strings::ToString(maxTickDeltaTime.AsMilliseconds()));
	pflr_maxDT.BindCallback(
		[](const String& max) { maxTickDeltaTime = Time::Milliseconds(Strings::ToS32(max)); });
	pflr_maxFT.Set(Strings::ToString(maxFrameDeltaTime.AsMilliseconds()));
	pflr_maxFT.BindCallback(
		[](const String& max) { maxFrameDeltaTime = Time::Milliseconds(Strings::ToS32(max)); });
#endif
}

void Toggle(bool bEnable)
{
	bEnabled = bEnable;
	uRenderer->SetEnabled(bEnable);
}

void Cleanup()
{
	safeThreadID = std::thread::id();
	uRenderer->Clear();
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
		uRenderer->Start(std::move(id), colour, maxTickDeltaTime, bEnabled);
	}
}

void StartFramed(String id, Colour colour)
{
	if (uRenderer)
	{
		uRenderer->Start(std::move(id), colour, maxFrameDeltaTime, bEnabled);
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
