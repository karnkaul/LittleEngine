#pragma once
#include "LittleEngine/LittleEngine.h"

namespace LittleEngine
{
class TestWorld : public World
{
public:
	TestWorld();

private:
	virtual void OnActivated() override;
	virtual void Tick(Time dt) override;
	virtual void OnDeactivating() override;
};
} // namespace LittleEngine
