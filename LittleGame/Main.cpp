#include "stdafx.h"
#include "LittleEngine/LittleEngine.h"
#include "Worlds/BootWorld.h"
#include "Worlds/TestWorld.h"

using namespace LittleEngine;

bool bInfiniteLoad = false;
bool bInfiniteBoot = false;

s32 TestGameLoop()
{
#if DEBUGGING
	WorldStateMachine::s_bTEST_infiniteLoad = bInfiniteLoad;
#endif
	auto uGameLoop = EngineLoop::Create();
	if (uGameLoop)
	{
		uGameLoop->Worlds()->CreateWorld<BootWorld>();
		uGameLoop->Worlds()->CreateWorld<TestWorld>();
		return uGameLoop->Run();
	}
	return -1;
}

s32 TestInfiniteBoot()
{
	s32 ret = -1;
#if DEBUGGING
	BootWorld::s_bTerminateOnFirstTick = true;
#endif
	while (true)
	{
		auto uEngine = EngineLoop::Create();
		uEngine->Worlds()->CreateWorld<BootWorld>();
		uEngine->Worlds()->CreateWorld<TestWorld>();
		ret = uEngine->Run();
	}
}

int main()
{
	if (bInfiniteBoot)
	{
		TestInfiniteBoot();
	}
	return TestGameLoop();
}
