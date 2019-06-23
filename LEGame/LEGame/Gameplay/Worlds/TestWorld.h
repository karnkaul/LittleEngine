#pragma once
#include "LEGame/Model/World/World.h"

namespace LittleEngine
{
class TestWorld : public World
{
public:
	TestWorld();

private:
	void OnActivated() override;
	void PreTick(Time dt) override;
	void PostTick(Time dt) override;
	void OnDeactivating() override;

	bool OnInput(const LEInput::Frame& frame);
};
} // namespace LittleEngine
