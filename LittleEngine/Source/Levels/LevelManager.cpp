#include "stdafx.h"
#include "LevelManager.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Engine.h"

#include "Level.h"
#include "BootLevel.h"
#include "TestLevel.h"

namespace LittleEngine {
	LevelManager::LevelManager(Engine& engine) : Object("LevelManager") {
		this->engine = &engine;
		Logger::Log(*this, "LevelManager created");
	}

	LevelManager::~LevelManager() {
		activeLevel = nullptr;
		Logger::Log(*this, "LevelManager destroyed");
	}

	Level& LevelManager::GetActiveLevel() const {
		return *activeLevel;
	}

	bool LevelManager::LoadLevel(const LevelID& levelID) {
		activeLevel = nullptr;
		switch (levelID) {
		case LevelID::BootLevel:
		default:
			activeLevel = std::make_unique<BootLevel>(*engine);
			return true;
			
		case LevelID::TestLevel:
			activeLevel = std::make_unique<TestLevel>(*engine);
			return true;
		}
		return false;
	}
}
