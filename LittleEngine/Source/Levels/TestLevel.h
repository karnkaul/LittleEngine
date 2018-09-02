#pragma once
#include "Level.h"
#include "Utils/Delegate.hpp"

namespace Game {
	class Engine;
	struct RenderParams;

	class TestLevel : public Level {
	public:
		TestLevel(Engine& engine);
		virtual void Render(RenderParams& params) override;
	private:
		Action::Token quitLevelToken;
		void OnQuitPressed();
	};
}
