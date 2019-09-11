#pragma once
#include "Model/World/World.h"

namespace LittleEngine
{
class TestWorld : public World
{
public:
	TestWorld();

protected:
	void OnActivated() override;
	void Tick(Time dt) override;
	void OnDeactivating() override;
};
} // namespace LittleEngine
