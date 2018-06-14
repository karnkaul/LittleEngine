#include "stdafx.h"
#include "LevelManager.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Engine.h"

#include "Level.h"
#include "BootLevel.h"
#include "TestLevel.h"

namespace Game {
	LevelManager::LevelManager(Engine& engine) : Object("LevelManager") {
		this->engine = &engine;
		Logger::Log(*this, "LevelManager created");
	}

	LevelManager::~LevelManager() {
		activeLevel = nullptr;
		Logger::Log(*this, "LevelManager destroyed");
	}

	Level& LevelManager::GetActiveLevel() {
		return *activeLevel;
	}

	bool LevelManager::LoadLevel(int levelIndex) {
		activeLevel = nullptr;
		switch (levelIndex) {
		case 0:
		default:
			activeLevel = std::make_unique<BootLevel>(*engine);
			return true;
			
		case 1:
			activeLevel = std::make_unique<TestLevel>(*engine);
			return true;
		}
		return false;
	}
}
