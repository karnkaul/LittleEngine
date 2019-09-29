#include "Core/FileRW.h"
#include "Core/Utils.h"
#include "SFMLAPI/Viewport/Viewport.h"
#include "Engine/GFX.h"
#include "Engine/Renderer/LERenderer.h"
#include "GameSettings.h"

namespace LittleEngine
{
namespace
{
const String VIEWPORT_HEIGHT_KEY = "RESOLUTION";
const String BORDERLESS_KEY = "BORDERLESS";
const String LOG_LEVEL_KEY = "LOG_LEVEL";
const String LOCALE_KEY = "LOCALE";
} // namespace

const String GameSettings::FILE_PATH = "Settings.txt";

GameSettings* GameSettings::Instance()
{
	static GameSettings s_gameSettings;
	return &s_gameSettings;
}

GameSettings::GameSettings()
{
	static const String FULL_PATH = String(OS::Env()->RuntimePath()) + "/" + FILE_PATH;
	SAVE_PATH = FULL_PATH;
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

ViewportSize GameSettings::SafeGetViewportSize()
{
	auto pMaxSize = g_pGFX->MaxViewportSize(IsBorderless());
	u32 h = ViewportHeight();
	u32 w = static_cast<u32>(g_pGFX->NativeAspectRatio() * h);
	if (h > pMaxSize->height || w > pMaxSize->width)
	{
		if (w > pMaxSize->width)
		{
			w = pMaxSize->width;
		}
		if (h > pMaxSize->height)
		{
			h = pMaxSize->height;
		}
		SetViewportHeight(h);
	}
	return ViewportSize(w, h);
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
	ViewportSize native = Viewport::MaxSize();
	m_viewportHeight = Property(VIEWPORT_HEIGHT_KEY, Strings::ToString(native.height) + "p");
	m_borderless = Property(BORDERLESS_KEY, Strings::ToString(true));
	m_logLevel = Property(LOG_LEVEL_KEY, String(Core::ParseLogSeverity(LogSeverity::Info)));
	m_locale = Property(LOCALE_KEY, "en");
}

void GameSettings::LoadAndOverride()
{
	if (m_persistor.Load(SAVE_PATH))
	{
		auto load = [&](Property& target, const String& key) {
			if (auto pSaved = m_persistor.GetProp(key))
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
	m_persistor.Save(SAVE_PATH);
}
} // namespace LittleEngine