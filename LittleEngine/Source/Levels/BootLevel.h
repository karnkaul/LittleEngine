#pragma once
#include "Level.h"
#include "Engine/Input/InputHandler.h"

namespace LittleEngine {
	class Engine;
	class TextRenderer;

	// \brief First level that Engine will load
	class BootLevel : public Level {
	public:
		BootLevel(Engine& engine);
		virtual void Tick(Fixed deltaTime) override;
	private:
		TextRenderer* logoRenderer;
		Actor::wPtr _logo;
		std::vector<OnInput::Token> inputTokens;

		void LoadAssets();
		void OnLoadNextLevel();
		void OnQuit();
	};
}
