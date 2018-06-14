#pragma once
#include "Level.h"

namespace Game {
	class Engine;
	class Actor;
	class TextRenderer;

	class BootLevel : public Level {
	public:
		BootLevel(Engine& engine);
		virtual void Tick(Fixed deltaTime) override;
	private:
		TextRenderer* logoRenderer;
		std::shared_ptr<Actor> logo;
		void LoadAssets();
	};
}
