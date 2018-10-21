#pragma once
#include "Levels/Level.h"
#include "Engine/Input/InputHandler.h"

namespace LittleEngine {
	class Engine;
	class TextRenderer;

	// \brief First level that Engine will load
	class BootLevel : public Level {
	public:
		BootLevel() : Level("BootLevel") {};

		virtual void LoadAssets() override;
		virtual void Tick(Fixed deltaTime) override;

	protected:
		virtual void Activate() override;

	private:
		TextRenderer* logoRenderer;
		Actor::wPtr _logo;
		std::vector<OnInput::Token> inputTokens;

		void OnLoadNextLevel();
		void OnQuit();
	};
}
