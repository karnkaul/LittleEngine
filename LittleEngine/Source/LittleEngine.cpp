#include "stdafx.h"
#include "Engine/Engine.h"

using namespace LittleEngine;

int main() {
	Engine::Ptr engine = Engine::Create();
	return engine->Run();
}
