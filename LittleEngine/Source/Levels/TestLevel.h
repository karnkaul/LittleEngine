#pragma once
#include "Level.h"
#include "Utils/Delegate.hpp"

namespace LittleEngine {
	class Engine;
	struct RenderParams;

	class TestLevel : public Level {
	public:
		TestLevel(Engine& engine);
		virtual void Render(RenderParams& params) override;

	private:
		Utils::Delegate<>::Token quitLevelToken;
		void OnQuitPressed();
	};
}
