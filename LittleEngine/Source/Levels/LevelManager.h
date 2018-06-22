#pragma once
#include "Engine/Object.h"

namespace Game {
	class Engine;
	class Level;

	class LevelManager : public Object {
	public:
		LevelManager(Engine& engine);
		~LevelManager();
		Level& GetActiveLevel();
		bool LoadLevel(int levelIndex);
	private:
		friend class Engine;
		Engine& engine;
		std::unique_ptr<Level> activeLevel;
	};
}
