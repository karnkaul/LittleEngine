#include "stdafx.h"
#include "Engine/Engine.h"

using namespace Game;

int main() {
	Engine::Ptr engine = Engine::Create();
	int returnCode = engine->Run();
	return returnCode;
}
