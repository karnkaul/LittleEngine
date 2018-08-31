#pragma once
#include "Engine/Object.h"

namespace Game {
	class Engine;
	class Level;

	class LevelManager final : public Object {
	public:
		LevelManager(Engine& engine);
		~LevelManager();
		Level& GetActiveLevel() const;
		bool LoadLevel(int levelIndex);
	private:
		Engine* engine;
		std::unique_ptr<Level> activeLevel;
	};
}
