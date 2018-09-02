#pragma once
#include "Engine/Object.h"

namespace Game {
	class Engine;
	class Level;
	enum class LevelID;

	// \brief Owner of currently active level that Engine is updating
	class LevelManager final : public Object {
	public:
		LevelManager(Engine& engine);
		~LevelManager();

		Level& GetActiveLevel() const;
		bool LoadLevel(const LevelID& levelID);

	private:
		Engine* engine;
		std::unique_ptr<Level> activeLevel;
	};
}
