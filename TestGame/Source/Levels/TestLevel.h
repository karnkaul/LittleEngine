#pragma once
#include "Levels/Level.h"
#include "Delegate.hpp"

namespace LittleEngine {
	class TestLevel : public Level {
	public:
		TestLevel() : Level("TestLevel") {}

		virtual void LoadAssets() override;
		virtual void PostRender(const RenderParams& params) override;

	protected:
		virtual void OnActivated() override;
		virtual void OnClearing() override;

	private:
		void OnQuitPressed();
	};
}
