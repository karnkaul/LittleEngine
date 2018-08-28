#include "stdafx.h"
#include <vector>
#include "EngineCommands.h"
#include "Engine.h"
//#include "Levels/Level.h"
#include "Levels/LevelManager.h"

namespace Game {
	LoadLevelCommand::LoadLevelCommand(LevelManager& levelManager, int levelID) : levelManager(levelManager), levelID(levelID) {}

	bool LoadLevelCommand::operator()() {
		return levelManager.LoadLevel(levelID);
	}
}
