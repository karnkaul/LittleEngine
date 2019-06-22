#pragma once
#include "Core/CoreTypes.h"
#include "Core/Property.h"
#include "Core/Logger.h"
#include "SFMLAPI/Viewport/SFViewportData.h"

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
	Property m_viewportHeight;
	Property m_borderless;
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

	SFViewportStyle GetViewportStyle() const;
	SFViewportSize GetViewportSize(Vector2 viewSize) const;
	Vector2 GetCullBounds(Vector2 viewSize) const;

	template <typename T>
	T GetProperty(const String& id);

private:
	void SetDefaults();
	void LoadAndOverride();
	void SaveAll();
};

template <typename T>
T GameSettings::GetProperty(const String& id)
{

}
} // namespace LittleEngine
