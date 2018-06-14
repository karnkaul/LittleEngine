#pragma once

namespace Game {
	class Engine;

	class EngineCommand {
	public:
		virtual ~EngineCommand() = default;
		virtual bool Execute() = 0;
	protected:
		EngineCommand() = default;
	};

	class LoadLevelCommand : public EngineCommand {
	public:
		LoadLevelCommand(Engine& engine, int levelID);
		virtual bool Execute() override;
	private:
		Engine* engine;
		int levelID;
	};
}
