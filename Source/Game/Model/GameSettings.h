#pragma once
#include <optional>
#include "Core/LECoreGame/LECoreUtils/Logger.h"
#include "Core/LECoreGame/Property.h"
#include "Core/CoreTypes.h"
#include "SFMLAPI/Viewport/ViewportData.h"

namespace LittleEngine
{
using Property = Core::Property;
using LogSeverity = Core::LogSeverity;

// \brief Wrapper to maintain properties saved to / loaded from Settings.txt
class GameSettings final
{
public:
	static const std::string FILE_PATH;

private:
	std::string SAVE_PATH;
	Property::Persistor m_persistor;
	Property m_viewportHeight;
	Property m_borderless;
	Property m_logLevel;
	Property m_locale;
	bool m_bAutoSave = false;

public:
	static GameSettings* Instance();

public:
	GameSettings();
	~GameSettings();

	u32 ViewportHeight() const;
	bool IsBorderless() const;

	void SetViewportHeight(u32 height);
	void SetBorderless(bool bBorderless);
	void SetLogLevel(LogSeverity level);

	ViewportStyle GetViewportStyle() const;
	ViewportSize SafeGetViewportSize();
	LogSeverity LogLevel() const;
	std::string LocdataID() const;
	const std::string& ENLocdataID() const;

	const std::string* GetValue(const std::string& key) const;

private:
	void SetDefaults();
	void LoadAndOverride();
	void SaveAll();
};
} // namespace LittleEngine
