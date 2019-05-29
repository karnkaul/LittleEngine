#include "stdafx.h"
#include "Core/FileRW.h"
#include "Core/Utils.h"
#include "SFMLAPI/Viewport/SFViewport.h"
#include "GameSettings.h"

namespace LittleEngine
{
namespace
{
const char* FILE_PATH = "Settings.ini";

const char* VIEWPORT_HEIGHT_KEY = "RESOLUTION";
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

u32 GameSettings::GetViewportHeight() const
{
	String heightStr = m_viewportHeight.stringValue.substr(0, m_viewportHeight.stringValue.size() - 1);
	s32 height = Strings::ToS32(heightStr);
	s32 maxHeight = SFViewport::GetMaxSize().height;
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

SFViewportStyle GameSettings::GetViewportStyle() const
{
	SFViewportStyle ret = SFViewportStyle::Default;
	if (IsBorderless())
	{
		ret = SFViewportStyle::Bordlerless;
	}
	return ret;
}

SFViewportSize GameSettings::GetViewportSize(Vector2 viewSize) const
{
	u32 h = GetViewportHeight();
	u32 w = (viewSize.x.ToU32() * h) / viewSize.y.ToU32();
	return {w, h};
}

Vector2 GameSettings::GetCullBounds(Vector2 viewSize) const
{
	SFViewportSize size = GetViewportSize(viewSize);
	return Vector2(size.height, size.height);
}

void GameSettings::SetDefaults()
{
	m_viewportHeight = Property(VIEWPORT_HEIGHT_KEY, Strings::ToString(1080) + "p");
	m_borderless = Property(BORDERLESS_KEY, Strings::ToString(false));
}

void GameSettings::LoadAndOverride()
{
	Property::Persistor persistor;
	if (persistor.Load(FILE_PATH))
	{
		auto pSaved = persistor.GetProp(VIEWPORT_HEIGHT_KEY);
		if (pSaved)
		{
			m_viewportHeight = *pSaved;
		}

		pSaved = persistor.GetProp(BORDERLESS_KEY);
		if (pSaved)
		{
			m_borderless = *pSaved;
		}
	}
}

void GameSettings::SaveAll()
{
	Property::Persistor persistor;
	persistor.SetProp(m_viewportHeight);
	persistor.SetProp(m_borderless);
	persistor.Save(FILE_PATH);
}
} // namespace LittleEngine
