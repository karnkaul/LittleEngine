#pragma once
#include "LEGame/Model/World/World.h"

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
	void OnActivated() override;
	void PreTick(Time dt) override;
	void PostTick(Time dt) override;
	void OnDeactivating() override;

	void OnLoadNextWorld();
};
} // namespace LittleEngine
