#pragma once
#include "Engine/Object.h"
#include "Level.h"

namespace LittleEngine {
	class Engine;
	
	// \brief Owner of currently active level that Engine is updating
	class LevelManager final : public Object {
	private:
		std::vector<std::unique_ptr<Level>> m_createdLevels;
		Engine* m_pEngine;
		Level* m_pActiveLevel = nullptr;
		LevelID m_activeLevelID = -1;

	public:
		LevelManager(Engine& engine);
		~LevelManager();

		Level* GetActiveLevel() const;
		LevelID GetActiveLevelID() const;
		bool LoadLevel(const LevelID& levelID);

		// \brief Returns level index
		template<typename T>
		LevelID CreateLevel() {
			static_assert(std::is_base_of<Level, T>::value, "T must derive from Level");
			std::unique_ptr<T> uLevel = std::make_unique<T>();
			if (uLevel) {
				uLevel->SetEngine(*m_pEngine);
				uLevel->LoadAndSpawn();
				m_createdLevels.push_back(std::move(uLevel));
			}
			return static_cast<LevelID>(m_createdLevels.size() - 1);
		}
	};
}
