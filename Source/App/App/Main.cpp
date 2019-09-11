#include "Gameplay/GameLoop.h"
#include "Gameplay/GameInit.h"

using namespace LittleEngine;

int main(int argc, char** argv)
{
	GameInit::CreateWorlds();
	return GameLoop::Run(argc, argv);
}
