#include "le_stdafx.h"
#include "LevelManager.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Engine.h"
#include "Level.h"

namespace LittleEngine {
	LevelManager::LevelManager(Engine& engine) : Object("LevelManager"), m_pEngine(&engine) {
		Logger::Log(*this, "LevelManager created");
	}

	LevelManager::~LevelManager() {
		m_createdLevels.clear();
		Logger::Log(*this, "LevelManager destroyed");
	}

	Level* LevelManager::GetActiveLevel() const {
		return m_pActiveLevel;
	}

	LevelID LevelManager::GetActiveLevelID() const {
		return m_activeLevelID;
	}

	bool LevelManager::LoadLevel(const LevelID& levelID) {
		if (levelID >= 0 && static_cast<size_t>(levelID) < m_createdLevels.size()) {
			if (m_pActiveLevel) {
				m_pActiveLevel->Clear();
			}
			m_activeLevelID = levelID;
			m_pActiveLevel = m_createdLevels[m_activeLevelID].get();
			m_pActiveLevel->Activate();
			return true;
		}
		return false;
	}
}
