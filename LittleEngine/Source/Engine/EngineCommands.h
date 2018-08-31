#pragma once

namespace Game {
	class LevelManager;

	class EngineCommand {
	public:
		virtual ~EngineCommand() = default;
		virtual bool operator()() = 0;
	protected:
		EngineCommand() = default;
	};

	class LoadLevelCommand : public EngineCommand {
	public:
		LoadLevelCommand(LevelManager& levelManager, int levelID);
		virtual bool operator()() override;
	private:
		LevelManager* levelManager;
		int levelID;
	};
}
