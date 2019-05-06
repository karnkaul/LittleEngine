#include "stdafx.h"
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
	if (uGameLoop)
	{
		uGameLoop->Worlds()->CreateWorld<BootWorld>();
		uGameLoop->Worlds()->CreateWorld<TestWorld>();
		return uGameLoop->Run();
	}
	return -1;
}

int main(int argc, char** argv)
{
	OS::Env()->SetVars(argc, argv);
	return TestGameLoop();
}
