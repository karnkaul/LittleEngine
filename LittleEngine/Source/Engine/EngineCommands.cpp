#include "stdafx.h"
#include <vector>
#include "EngineCommands.h"
#include "Engine.h"
#include "Levels/Level.h"
#include "Levels/LevelManager.h"

namespace Game {
	LoadLevelCommand::LoadLevelCommand(Engine& engine, int levelID) : engine(engine), levelID(levelID) {}

	bool LoadLevelCommand::Execute() {
		return engine.levelManager->LoadLevel(levelID);
	}
}
