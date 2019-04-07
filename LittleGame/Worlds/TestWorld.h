#pragma once
#include "LittleEngine/LittleEngine.h"

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
};
} // namespace LittleEngine
