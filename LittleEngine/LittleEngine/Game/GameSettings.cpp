#include "stdafx.h"
#include "FileRW.h"
#include "Utils.h"
#include "SFMLAPI/Windowing/SFWindow.h"
#include "SFMLAPI/Windowing/SFWindowData.h"
#include "GameSettings.h"

namespace LittleEngine
{
namespace
{
const char* FILE_PATH = "Settings.ini";

const char* WINDOW_HEIGHT_KEY = "RESOLUTION";
const char* BORDERLESS_KEY = "BORDERLESS";
} // namespace

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

u32 GameSettings::GetWindowHeight()
{
	String heightStr = m_windowHeight.stringValue.substr(0, m_windowHeight.stringValue.size() - 1);
	s32 height = Strings::ToS32(heightStr);
	s32 maxHeight = SFWindow::GetMaxWindowSize().height;
	if (height < 0 || height > maxHeight)
	{
		height = 720;
	}
	return static_cast<u32>(height);
}

bool GameSettings::IsBorderless()
{
	return Strings::ToBool(m_borderless.stringValue);
}

void GameSettings::SetWindowHeight(u32 height)
{
	m_windowHeight.stringValue = Strings::ToString(height) + "p";
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

void GameSettings::SetDefaults()
{
	m_windowHeight = Property(WINDOW_HEIGHT_KEY, Strings::ToString(1080) + "p");
	m_borderless = Property(BORDERLESS_KEY, Strings::ToString(false));
}

void GameSettings::LoadAndOverride()
{
	Property::Persistor persistor;
	if (persistor.Load(FILE_PATH))
	{
		Property saved = persistor.GetProp(WINDOW_HEIGHT_KEY);
		if (saved.key == WINDOW_HEIGHT_KEY)
		{
			m_windowHeight = saved;
		}

		saved = persistor.GetProp(BORDERLESS_KEY);
		if (saved.key == BORDERLESS_KEY)
		{
			m_borderless = saved;
		}
	}
}

void GameSettings::SaveAll()
{
	Property::Persistor persistor;
	persistor.SetProp(m_windowHeight);
	persistor.SetProp(m_borderless);
	persistor.Save(FILE_PATH);
}
} // namespace LittleEngine
