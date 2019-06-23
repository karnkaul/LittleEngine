#include "stdafx.h"
#include "GameInit.h"
#include "LittleEngine/Renderer/LEShaders.h"
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

void GameInit::LoadShaders()
{
	// Load SFShaders here

	LEShaders::LoadShader<Shader>("Default", ShaderType::VertFrag);
}
} // namespace LittleEngine