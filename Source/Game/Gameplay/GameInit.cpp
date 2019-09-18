#include "GameInit.h"
#include "Engine/Renderer/LEShaders.h"
#include "Model/World/WorldStateMachine.h"
#include "Gameplay/Worlds.h"

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

	LEShaders::LoadShader<Shader>("Default", Shader::Type().set(ToIdx(Shader::Flag::Vertex)).set(ToIdx(Shader::Flag::Fragment)));
}
} // namespace LittleEngine
