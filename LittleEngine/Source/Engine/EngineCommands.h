#pragma once

namespace Game {
	class Engine;

	class EngineCommand {
	public:
		virtual ~EngineCommand() = default;
		virtual bool operator()() = 0;
	protected:
		EngineCommand() = default;
	};

	class LoadLevelCommand : public EngineCommand {
	public:
		LoadLevelCommand(Engine& engine, int levelID);
		virtual bool operator()() override;
	private:
		Engine& engine;
		int levelID;
	};
}
