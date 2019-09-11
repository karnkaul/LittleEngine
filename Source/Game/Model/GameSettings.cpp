#include "Core/FileRW.h"
#include "Core/Utils.h"
#include "SFMLAPI/Viewport/Viewport.h"
#include "Engine/Renderer/LERenderer.h"
#include "GameSettings.h"

namespace LittleEngine
{
namespace
{
const char* VIEWPORT_HEIGHT_KEY = "RESOLUTION";
const char* BORDERLESS_KEY = "BORDERLESS";
const char* LOG_LEVEL_KEY = "LOG_LEVEL";
const char* LOCALE_KEY = "LOCALE";
} // namespace

const char* GameSettings::szFILE_PATH = "Settings.ini";

GameSettings* GameSettings::Instance()
{
	static GameSettings s_gameSettings;
	return &s_gameSettings;
}

GameSettings::GameSettings()
{
	static const String FULL_PATH = (OS::Env()->RuntimePath() + "/" + szFILE_PATH);
	szFILE_PATH = FULL_PATH.c_str();
	SetDefaults();
	LoadAndOverride();
	SaveAll();
	m_bAutoSave = true;
}

GameSettings::~GameSettings()
{
	SaveAll();
}

u32 GameSettings::ViewportHeight() const
{
	String heightStr = m_viewportHeight.stringValue.substr(0, m_viewportHeight.stringValue.size() - 1);
	s32 height = Strings::ToS32(heightStr);
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

ViewportSize GameSettings::SafeGetViewportSize(Vector2 viewSize)
{
	LERenderer::PopulateViewportSizes(viewSize.x / viewSize.y);
	auto pMaxSize = LERenderer::MaxViewportSize(IsBorderless());
	u32 h = ViewportHeight();
	u32 w = (viewSize.x.ToU32() * h) / viewSize.y.ToU32();
	if ((w + 1) % 10 == 0)
	{
		w += 1;
	}
	if (h > pMaxSize->height || w > pMaxSize->width)
	{
		f32 screenAspect = pMaxSize->width / static_cast<f32>(pMaxSize->height);
		f32 viewAspect = (viewSize.x / viewSize.y).ToF32();
		if (viewAspect > screenAspect)
		{
			w = pMaxSize->width;
			h = static_cast<u32>(w / viewAspect);
			if ((h + 1) % 10 == 0)
			{
				h += 1;
			}
		}
		else
		{
			h = pMaxSize->height;
			w = static_cast<u32>(h * viewAspect);
			if ((w + 1) % 10 == 0)
			{
				w += 1;
			}
		}
		SetViewportHeight(h);
	}
	return {w, h};
}

LogSeverity GameSettings::LogLevel() const
{
	return Core::ParseLogSeverity(m_logLevel.stringValue);
}

String GameSettings::LocdataID() const
{
	return "Texts/Locale/" + m_locale.stringValue + ".loc";
}

const String& GameSettings::ENLocdataID() const
{
	static String enLocdataID = "Texts/Locale/en.loc";
	return enLocdataID;
}

const String* GameSettings::GetValue(const String& key) const
{
	auto pProp = m_persistor.GetProp(key);
	if (pProp)
	{
		return &pProp->stringValue;
	}
	return nullptr;
}

void GameSettings::SetDefaults()
{
	m_viewportHeight = Property(VIEWPORT_HEIGHT_KEY, Strings::ToString(1080) + "p");
	m_borderless = Property(BORDERLESS_KEY, Strings::ToString(false));
	m_logLevel = Property(LOG_LEVEL_KEY, Core::ParseLogSeverity(LogSeverity::Info));
	m_locale = Property(LOCALE_KEY, "en");
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
		load(m_locale, LOCALE_KEY);
	}
}

void GameSettings::SaveAll()
{
	m_persistor.SetProp(m_viewportHeight);
	m_persistor.SetProp(m_borderless);
	m_persistor.SetProp(m_logLevel);
	m_persistor.SetProp(m_locale);
	m_persistor.Save(szFILE_PATH);
}
} // namespace LittleEngine
