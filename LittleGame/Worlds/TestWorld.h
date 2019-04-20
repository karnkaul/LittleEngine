#pragma once
#include "LittleEngine/Game/World/World.h"

namespace LittleEngine
{
class TestWorld : public World
{
public:
	TestWorld();

private:
	void OnActivated() override;
	void Tick(Time dt) override;
	void OnDeactivating() override;

	bool OnInput(const EngineInput::Frame& frame);
};
} // namespace LittleEngine
