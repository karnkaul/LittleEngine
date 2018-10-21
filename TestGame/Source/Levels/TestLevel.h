#pragma once
#include "Levels/Level.h"
#include "Delegate.hpp"

namespace LittleEngine {
	class Engine;
	struct RenderParams;

	class TestLevel : public Level {
	public:
		TestLevel() : Level("TestLevel") {}

		virtual void PostRender(const RenderParams& params) override;

	protected:
		virtual void Activate() override;
		virtual void OnClearing() override;

	private:
		void OnQuitPressed();
	};
}
