#include "le_stdafx.h"
#include "LevelManager.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Engine.h"

#include "Level.h"
#include "BootLevel.h"
#include "TestLevel.h"

namespace LittleEngine {
	LevelManager::LevelManager(Engine& engine) : Object("LevelManager"), engine(&engine) {
		Logger::Log(*this, "LevelManager created");
	}

	LevelManager::~LevelManager() {
		createdLevels.clear();
		Logger::Log(*this, "LevelManager destroyed");
	}

	Level* LevelManager::GetActiveLevel() const {
		return activeLevel;
	}

	LevelID LevelManager::GetActiveLevelID() const {
		return activeLevelID;
	}

	bool LevelManager::LoadLevel(const LevelID& levelID) {
		if (levelID >= 0 && levelID < createdLevels.size()) {
			if (activeLevel) {
				activeLevel->Clear();
			}
			activeLevelID = levelID;
			activeLevel = &(*createdLevels[activeLevelID]);
			activeLevel->Activate();
			return true;
		}
		return false;
	}
}
