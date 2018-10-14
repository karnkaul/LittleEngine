#include "stdafx.h"
#include <vector>
#include "EngineCommands.h"
#include "Engine.h"
#include "Levels/LevelManager.h"
#include "Engine/Audio/AudioManager.h"

namespace LittleEngine {
	LoadLevelCommand::LoadLevelCommand(LevelManager& levelManager, const LevelID& levelID) 
		: levelID(levelID), levelManager(&levelManager) 
	{}

	bool LoadLevelCommand::operator()() {
		return levelManager->LoadLevel(levelID);
	}
}
