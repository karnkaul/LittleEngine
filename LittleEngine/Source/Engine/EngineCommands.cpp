#include "stdafx.h"
#include <vector>
#include "EngineCommands.h"
#include "Engine.h"
#include "Levels/LevelManager.h"
#include "Engine/Audio/AudioManager.h"

namespace LittleEngine {
	LoadLevelCommand::LoadLevelCommand(LevelManager& levelManager, AudioManager & audioManager, const LevelID& levelID) {
		this->levelID = levelID;
		this->levelManager = &levelManager;
		this->audioManager = &audioManager;
	}

	bool LoadLevelCommand::operator()() {
		audioManager->Clear(false);
		return levelManager->LoadLevel(levelID);
	}
}
