#pragma once
#include "Levels/Level.h"
#include "Engine/Input/InputHandler.h"

namespace LittleEngine {
	// \brief First level that Engine will load
	class BootLevel : public Level {
	public:
		BootLevel() : Level("BootLevel") {};

		virtual void Tick(Fixed deltaTime) override;

	protected:
		virtual void OnActivated() override;

	private:
		class TextRenderable* logoRenderer;
		int logoID = 0;

		void OnLoadNextLevel();
	};
}
