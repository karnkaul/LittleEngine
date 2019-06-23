#include "stdafx.h"
#include "Core/FileRW.h"
#include "Core/Utils.h"
#include "SFMLAPI/Viewport/Viewport.h"
#include "GameSettings.h"

namespace LittleEngine
{
namespace
{
const char* VIEWPORT_HEIGHT_KEY = "RESOLUTION";
const char* BORDERLESS_KEY = "BORDERLESS";
const char* LOG_LEVEL_KEY = "LOG_LEVEL";
} // namespace

const char* GameSettings::szFILE_PATH = "Settings.ini";

GameSettings* GameSettings::Instance()
{
	static GameSettings s_gameSettings;
	return &s_gameSettings;
}

GameSettings::GameSettings()
{
	SetDefaults();
	LoadAndOverride();
	SaveAll();
	m_bAutoSave = true;
}

GameSettings::~GameSettings()
{
	SaveAll();
}

u32 GameSettings::GetViewportHeight() const
{
	String heightStr = m_viewportHeight.stringValue.substr(0, m_viewportHeight.stringValue.size() - 1);
	s32 height = Strings::ToS32(heightStr);
	s32 maxHeight = Viewport::GetMaxSize().height;
	if (height < 0 || height > maxHeight)
	{
		height = 720;
	}
	return static_cast<u32>(height);
}

bool GameSettings::IsBorderless() const
{
	return Strings::ToBool(m_borderless.stringValue);
}

void GameSettings::SetViewportHeight(u32 height)
{
	m_viewportHeight.stringValue = Strings::ToString(height) + "p";
	if (m_bAutoSave)
	{
		SaveAll();
	}
}

void GameSettings::SetBorderless(bool bBorderless)
{
	m_borderless.stringValue = Strings::ToString(bBorderless);
	if (m_bAutoSave)
	{
		SaveAll();
	}
}

void GameSettings::SetLogLevel(LogSeverity level)
{
	m_logLevel.stringValue = Core::ParseLogSeverity(level);
	if (m_bAutoSave)
	{
		SaveAll();
	}
}

ViewportStyle GameSettings::GetViewportStyle() const
{
	ViewportStyle ret = ViewportStyle::Default;
	if (IsBorderless())
	{
		ret = ViewportStyle::Bordlerless;
	}
	return ret;
}

ViewportSize GameSettings::GetViewportSize(Vector2 viewSize) const
{
	u32 h = GetViewportHeight();
	u32 w = (viewSize.x.ToU32() * h) / viewSize.y.ToU32();
	return {w, h};
}

Vector2 GameSettings::GetCullBounds(Vector2 viewSize) const
{
	ViewportSize size = GetViewportSize(viewSize);
	return Vector2(size.height, size.height);
}

LogSeverity GameSettings::GetLogLevel() const
{
	return Core::ParseLogSeverity(m_logLevel.stringValue);
}

String GameSettings::GetValue(const String& key) const
{
	auto pProp = m_persistor.GetProp(key);
	if (pProp)
	{
		return pProp->stringValue;
	}
	return {};
}

void GameSettings::SetDefaults()
{
	m_viewportHeight = Property(VIEWPORT_HEIGHT_KEY, Strings::ToString(1080) + "p");
	m_borderless = Property(BORDERLESS_KEY, Strings::ToString(false));
	m_logLevel = Property(LOG_LEVEL_KEY, Core::ParseLogSeverity(LogSeverity::Info));
}

void GameSettings::LoadAndOverride()
{
	if (m_persistor.Load(szFILE_PATH))
	{
		auto load = [&](Property& target, const char* szKey) {
			if (auto pSaved = m_persistor.GetProp(szKey))
			{
				target = *pSaved;
			}
		};

		load(m_viewportHeight, VIEWPORT_HEIGHT_KEY);
		load(m_borderless, BORDERLESS_KEY);
		load(m_logLevel, LOG_LEVEL_KEY);
	}
}

void GameSettings::SaveAll()
{
	m_persistor.SetProp(m_viewportHeight);
	m_persistor.SetProp(m_borderless);
	m_persistor.SetProp(m_logLevel);
	m_persistor.Save(szFILE_PATH);
}
} // namespace LittleEngine
