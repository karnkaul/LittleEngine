#pragma once
#include "Levels/Level.h"
#include "Engine/Input/InputHandler.h"

namespace LittleEngine {
	// \brief First level that Engine will load
	class BootLevel : public Level {
	private:
		class UIButtonDrawer* m_pLogoDrawer = nullptr;
		class UIElement* m_pLogoHeader = nullptr;
		class FontAsset* m_pLogoFont = nullptr;
		
	public:
		BootLevel();

		virtual void LoadAndSpawn() override;
		virtual void Tick(Fixed deltaMS) override;

	protected:
		virtual void OnActivated() override;

	private:
		void OnLoadNextLevel();
	};
}
