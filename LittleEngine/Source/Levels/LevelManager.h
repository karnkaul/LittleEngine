#pragma once
#include "Engine/Object.h"
#include "Level.h"

namespace LittleEngine {
	class Engine;
	
	// \brief Owner of currently active level that Engine is updating
	class LevelManager final : public Object {
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
			std::unique_ptr<T> pLevel = std::make_unique<T>();
			if (pLevel) {
				pLevel->SetEngine(*engine);
				pLevel->LoadAssets();
				createdLevels.push_back(std::move(pLevel));
			}
			return static_cast<LevelID>(createdLevels.size() - 1);
		}

	private:
		Engine* engine;
		Level* activeLevel = nullptr;
		LevelID activeLevelID = -1;

		std::vector<std::unique_ptr<Level>> createdLevels;
	};
}
