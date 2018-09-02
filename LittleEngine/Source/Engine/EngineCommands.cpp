#include "stdafx.h"
#include <vector>
#include "EngineCommands.h"
#include "Engine.h"
#include "Levels/LevelManager.h"

namespace Game {
	LoadLevelCommand::LoadLevelCommand(LevelManager& levelManager, const LevelID& levelID) {
		this->levelID = levelID;
		this->levelManager = &levelManager;
	}

	bool LoadLevelCommand::operator()() {
		return levelManager->LoadLevel(levelID);
	}
}
