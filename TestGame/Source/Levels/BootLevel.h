#pragma once
#include "Levels/Level.h"
#include "Engine/Input/InputHandler.h"

namespace LittleEngine {
	// \brief First level that Engine will load
	class BootLevel : public Level {
	private:
		class TextRenderable* m_pLogoRenderable;
		int m_logoID = 0;

	public:
		BootLevel() : Level("BootLevel") {};

		virtual void Tick(Fixed deltaMS) override;

	protected:
		virtual void OnActivated() override;

	private:
		void OnLoadNextLevel();
	};
}
