#include "stdafx.h"
#include "GameLoop/GameLoop.h"
#include "LEGame/GameModel/World/WorldStateMachine.h"
#include "LEGame/Worlds/BootWorld.h"
#include "LEGame/Worlds/TestWorld.h"

using namespace LittleEngine;

int main(int argc, char** argv)
{
	WorldStateMachine::CreateWorlds<BootWorld, TestWorld>();
	return GameLoop::Run(argc, argv);
}
