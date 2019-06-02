#include "stdafx.h"
#include "GameInit.h"
#include "LEGame/Framework.h"
#include "LEGame/Gameplay/Worlds.h"

namespace LittleEngine
{
void GameInit::CreateWorlds()
{
	// Create Worlds and manage their IDs (if required) here
	// WorldStateMachine::CreateWorld<T> will return the WorldID assigned to the created world.
	// WorldStateMachine::CreateWorlds<T...> will create worlds in order of template invocation and return nothing.
	
	WorldStateMachine::CreateWorlds<BootWorld, TestWorld>();
}
} // namespace LittleEngine