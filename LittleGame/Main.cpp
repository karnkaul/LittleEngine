#include "stdafx.h"
#include <iostream>
#include <thread>
#include "LittleEngine/LittleEngine.h"
#include "Worlds/BootWorld.h"
#include "Worlds/TestWorld.h"

using namespace LittleEngine;

bool bInfiniteLoad = false;

s32 TestGameLoop()
{
#if DEBUGGING
	WorldStateMachine::s_bTEST_infiniteLoad = bInfiniteLoad;
#endif
	auto uGameLoop = EngineLoop::Create();

	Services::Engine()->Worlds()->CreateWorld<BootWorld>();
	Services::Engine()->Worlds()->CreateWorld<TestWorld>();
	return uGameLoop->Run();
}

int main()
{
	return TestGameLoop();
}
