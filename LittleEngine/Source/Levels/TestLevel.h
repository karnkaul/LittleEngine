#pragma once
#include "Level.h"
#include "Utils/Delegate.hpp"

namespace LittleEngine {
	class Engine;
	struct RenderParams;

	using Action = Utils::Action;

	class TestLevel : public Level {
	public:
		TestLevel(Engine& engine);
		virtual void Render(RenderParams& params) override;
	private:
		Action::Token quitLevelToken;
		void OnQuitPressed();
	};
}
