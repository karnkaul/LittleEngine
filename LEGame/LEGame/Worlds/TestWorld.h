#pragma once
#include "LEGame/GameModel/World/World.h"

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

	bool OnInput(const LEInput::Frame& frame);
};
} // namespace LittleEngine
