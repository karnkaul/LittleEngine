#include "Core/Game/OS.h"
#include "Game/GameLoop.h"

using namespace LittleEngine;

s32 main(s32 argc, char** argv)
{
	OS::Env()->SetVars(argc, argv, {"Resources/GameAssets/Fonts/Default.ttf"});
	if (GameLoop::Init())
	{
		GameLoop::Run();
	}
	return 0;
}
