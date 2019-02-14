#pragma once
#include "LittleEngine/LittleEngine.h"

namespace LittleEngine
{
class BootWorld final : public World
{
private:
	class UIButtonDrawer* m_pLogoDrawer = nullptr;
	class UIElement* m_pLogoHeader = nullptr;
	class FontAsset* m_pLogoFont = nullptr;

public:
	BootWorld();

private:
	virtual void OnActivated() override;
	virtual void Tick(Time dt) override;
	virtual void OnDeactivating() override;

	void OnLoadNextWorld();
};
} // namespace LittleEngine
