#pragma once
#include "Level.h"

namespace Game {
	class Engine;
	struct RenderParams;

	class TestLevel : public Level {
	public:
		TestLevel(Engine& engine);
		~TestLevel();
		virtual void Render(RenderParams& params) override;
	};
}
