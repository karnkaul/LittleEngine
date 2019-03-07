#pragma once
#include "CoreTypes.h"
#include "Property.h"
#include "Logger.h"

namespace LittleEngine
{
using Property = Core::Property;
using LogSeverity = Core::LogSeverity;

// \brief Wrapper to maintain properties saved to / loaded from config.ini
class GameSettings final
{
private:
	Property m_windowHeight;
	Property m_borderless;
	bool m_bAutoSave = false;

public:
	static GameSettings* Instance();

public:
	GameSettings();
	~GameSettings();

	u32 GetWindowHeight();
	bool IsBordlerless();

	void SetWindowHeight(u32 height);
	void SetBorderless(bool bBordlerless);

private:
	void SetDefaults();
	void LoadAndOverride();
	void SaveAll();
};
} // namespace LittleEngine
