#pragma once
#include "Levels/Level.h"
#include "Delegate.hpp"

namespace LittleEngine {
	class TestLevel : public Level {
	public:
		TestLevel() : Level("TestLevel") {}

		virtual void LoadAndSpawn() override;
		virtual void Tick(Fixed deltaMS) override;
		virtual void PostRender() override;

	protected:
		virtual void OnActivated() override;
		virtual void OnClearing() override;

	private:
		void OnQuitPressed();
	};
}
