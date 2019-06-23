#pragma once
#include "Core/CoreTypes.h"
#include "Core/Property.h"
#include "Core/Logger.h"
#include "SFMLAPI/Viewport/ViewportData.h"

namespace LittleEngine
{
using Property = Core::Property;
using LogSeverity = Core::LogSeverity;

// \brief Wrapper to maintain properties saved to / loaded from config.ini
class GameSettings final
{
public:
	static const char* szFILE_PATH;

private:
	Property::Persistor m_persistor;
	Property m_viewportHeight;
	Property m_borderless;
	Property m_logLevel;
	bool m_bAutoSave = false;

public:
	static GameSettings* Instance();

public:
	GameSettings();
	~GameSettings();

	u32 GetViewportHeight() const;
	bool IsBorderless() const;

	void SetViewportHeight(u32 height);
	void SetBorderless(bool bBorderless);
	void SetLogLevel(LogSeverity level);

	ViewportStyle GetViewportStyle() const;
	ViewportSize GetViewportSize(Vector2 viewSize) const;
	Vector2 GetCullBounds(Vector2 viewSize) const;
	LogSeverity GetLogLevel() const;

	String GetValue(const String& key) const;

private:
	void SetDefaults();
	void LoadAndOverride();
	void SaveAll();
};
} // namespace LittleEngine
