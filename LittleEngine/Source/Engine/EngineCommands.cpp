#include "le_stdafx.h"
#include <vector>
#include "EngineCommands.h"
#include "Engine.h"
#include "Levels/LevelManager.h"
#include "Engine/Audio/AudioManager.h"

namespace LittleEngine {
	LoadLevelCommand::LoadLevelCommand(LevelManager& levelManager, const LevelID& levelID) : m_levelID(levelID), m_pLevelManager(&levelManager) {}

	bool LoadLevelCommand::operator()() {
		return m_pLevelManager->LoadLevel(m_levelID);
	}
}
