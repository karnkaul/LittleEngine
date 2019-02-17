#include "stdafx.h"
#include <iostream>
#include <thread>
#include "Logger.h"
#include "CoreTypes.h"
#include "Delegate.hpp"
#include "LittleEngine/LittleEngine.h"
#include "Worlds/BootWorld.h"
#include "Worlds/TestWorld.h"

using namespace LittleEngine;

s32 TestGameLoop()
{
	auto uGameLoop = EngineLoop::Create();

	Services::Engine()->Worlds()->CreateWorld<BootWorld>();
	Services::Engine()->Worlds()->CreateWorld<TestWorld>();
	return uGameLoop->Run();
}

int main()
{
	return TestGameLoop();
}
