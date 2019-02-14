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

void TestGameLoop()
{
	auto uGameLoop = EngineLoop::Create();

	Services::Engine()->Worlds()->CreateWorld<BootWorld>();
	Services::Engine()->Worlds()->CreateWorld<TestWorld>();
	s32 returnCode = uGameLoop->Run();

	StringStream s;
	s << "EngineLoop returned: " << returnCode;
	LogD(s);
}

int main()
{
	TestGameLoop();
	return 0;
}
