#include "stdafx.h"
#include "Levels/BootLevel.h"
#include "Levels/TestLevel.h"

using namespace LittleEngine;

int main() {
	// Instantiate engine
	Engine::Ptr engine = Engine::Create();
	
	// Add levels and load their assets
	engine->CreateLevel<BootLevel>();
	engine->CreateLevel<TestLevel>();
	
	// Run level 0
	ExitCode code = engine->Run();
	return static_cast<int>(code);
}
