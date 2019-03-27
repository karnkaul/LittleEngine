#include "stdafx.h"
#include "DebugProfiler.h"
#if ENABLED(PROFILER)
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/System/SFGameClock.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIProgressBar.h"
#include "LittleEngine/UI/UIText.h"

namespace LittleEngine
{
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

	Entry(const String& id, Colour colour, Time startTime);
	Time Elapsed() const;
};

Entry::Entry(const String& id, Colour colour, Time startTime)
	: progressBar(UIProgressBar(true)), labelElement(UIElement(true)), id(id), colour(colour), startTime(startTime)
{
}
Time Entry::Elapsed() const
{
	return endTime - startTime;
}

Vector2 progressBarSize;
Fixed textWidth;
Fixed profilerHeight;
Time maxDeltaTime;
u32 maxEntries = 30;
UByte globalAlpha = 200;

class Renderer
{
	UPtr<UIElement> m_uLabelRoot = nullptr;
	UPtr<UIElement> m_uBarRoot = nullptr;
	UMap<String, UPtr<Entry>> m_entries;

public:
	Renderer();

	void SetEnabled(bool bEnabled);
	void Tick(Time dt);
	void CaptureProfile();
	void Start(const String& id, Colour colour, bool bEnabled);
	void Stop(const String& id);
	void Clear();

private:
	void SetupNewEntry(u32 numEntries, Entry& newEntry, Colour colour, bool bEnabled);
};

Renderer::Renderer()
{
	m_uLabelRoot = MakeUnique<UIElement>("Profiler Labels Root", true);
	m_uLabelRoot->InitElement();
	m_uLabelRoot->m_transform.size = {textWidth, profilerHeight};
	m_uLabelRoot->m_transform.bAutoPad = true;
	m_uLabelRoot->m_transform.nPosition = {-1, -1};
	m_uLabelRoot->m_layer = LAYER_TOP;
	// m_uLabelRoot->SetPanel(Colour(100, 100, 100, 100));
	m_uBarRoot = MakeUnique<UIElement>("Profiler Bars Root", true);
	m_uBarRoot->InitElement();
	m_uBarRoot->m_transform.size = Vector2(progressBarSize.x, profilerHeight);
	m_uBarRoot->m_transform.bAutoPad = true;
	m_uBarRoot->m_transform.nPosition = {1, -1};
	m_uBarRoot->m_layer = LAYER_TOP;
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
}

void Renderer::CaptureProfile()
{
	Time now = Time::Now();
	for (auto& entry : m_entries)
	{
		if (entry.second->endTime == Time::Zero)
			entry.second->endTime = now;
		f64 progress = static_cast<f64>(entry.second->Elapsed().AsMicroseconds()) /
					   static_cast<f64>(maxDeltaTime.AsMicroseconds());
		entry.second->progressBar.SetProgress(Fixed(progress));
		entry.second->progressBar.Tick(Time::Zero);
	}
}

void Renderer::Start(const String& id, Colour colour, bool bEnabled)
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
		UPtr<Entry> uNewEntry = MakeUnique<Entry>(id, colour, Time::Now());
		SetupNewEntry(m_entries.size(), *uNewEntry, colour, bEnabled);
		m_entries.emplace(id, std::move(uNewEntry));
	}
}

void Renderer::Stop(const String& id)
{
	Assert(std::this_thread::get_id() == safeThreadID,
		   "Can only use Profiler on Engine Loop thread!");
	auto search = m_entries.find(id);
	if (search != m_entries.end())
	{
		search->second->endTime = Time::Now();
	}
}

void Renderer::Clear()
{
	m_entries.clear();
}

void Renderer::SetupNewEntry(u32 numEntries, Entry& newEntry, Colour colour, bool bEnabled)
{
	Fixed numerator = maxEntries - numEntries;
	Fixed nY = numerator / Fixed(static_cast<f32>(maxEntries)) - Fixed(0.03f);
	nY = (nY * 2) - 1;
	colour.a = globalAlpha;
	newEntry.progressBar.InitElement(&m_uBarRoot->m_transform);
	newEntry.progressBar.InitProgressBar(progressBarSize, colour);
	newEntry.progressBar.m_layer = LAYER_TOP;
	newEntry.progressBar.m_transform.anchor = {-1, 0};
	newEntry.progressBar.m_transform.nPosition = {-1, nY};
	newEntry.progressBar.GetPrimitive()->SetEnabled(bEnabled);
	newEntry.labelElement.InitElement(&m_uLabelRoot->m_transform);
	newEntry.labelElement.SetText(UIText(newEntry.id, 20, newEntry.colour));
	newEntry.labelElement.m_layer = LAYER_TOP;
	newEntry.labelElement.m_transform.anchor = {1, 0};
	newEntry.labelElement.m_transform.nPosition = {1, nY};
	newEntry.labelElement.GetText()->SetEnabled(bEnabled);
}

using URenderer = UPtr<Renderer>;

URenderer uRenderer = nullptr;
bool bEnabled = false;
} // namespace
#pragma endregion

#pragma region Interface
void Init(std::thread::id eventThreadID)
{
	safeThreadID = eventThreadID;
	maxDeltaTime = Time::Milliseconds(50);
	textWidth = 300;
	Vector2 worldSize = GFX::GetViewSize();
	profilerHeight = worldSize.y * Fixed(2, 3);
	Fixed screenWidth = worldSize.x;
	progressBarSize = Vector2(screenWidth - textWidth - 10, 10);
	uRenderer = MakeUnique<Renderer>();
	Toggle(false);
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
		uRenderer->Tick(dt);
}

void Render()
{
	if (bEnabled && uRenderer)
		uRenderer->CaptureProfile();
}

void Reset()
{
	if (uRenderer)
		uRenderer->Clear();
}

void Start(const String& id, Colour colour)
{
	if (uRenderer)
		uRenderer->Start(id, colour, bEnabled);
}

void Stop(const String& id)
{
	if (uRenderer)
		uRenderer->Stop(id);
}
#pragma endregion
} // namespace Profiler
} // namespace Debug
} // namespace LittleEngine
#endif
