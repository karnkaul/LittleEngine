#pragma once
#include <vector>
#include "Object.h"
#include "Utils/Fixed.h"
#include "SFMLInterface/SystemClock.h"

namespace Consts {
	constexpr int MAX_FIXED_TICKS = 20;
	constexpr int MS_PER_FIXED_TICK = 6;
	constexpr int MAX_FPS = 125;
}

namespace Game {
	class Actor;
	class World;
	class InputHandler;
	class EngineConfig;
	class AssetManager;
	class Level;
	class LevelManager;
	class WindowController;
	class EngineCommand;

	enum class ExitCode { 
		OK,
		InitError,
		DependencyError,
		AllocationError,
		ExecutionError,
		CleanupError
	};
	
	class Engine : Object {
	public:
		using Ptr = std::unique_ptr<Engine>;
		static Ptr Create();
		Engine(const std::string&) = delete;
		~Engine();
		// Copy constructor and copy assignment operators
		// will be auto-deleted due to unique_ptr members

		// Returns exit code
		int Run();

		const InputHandler& GetInputHandler() const;
		const World& GetWorld() const;
		AssetManager& GetAssetManager() const;
		void LoadLevel(int id);

	private:
		std::unique_ptr<AssetManager> assetManager;
		std::unique_ptr<LevelManager> levelManager;
		std::unique_ptr<EngineConfig> config;
		std::unique_ptr<World> world;
		std::unique_ptr<WindowController> windowController;
		std::vector<std::unique_ptr<EngineCommand> > commands;
		SystemClock clock;
		ExitCode exitCode = ExitCode::OK;
		bool isPaused = false;

		Engine();

		void PollInput();
		void FixedTick(Fixed& lag);
		void Tick(Fixed deltaTime);
		void Render();
		void PostRender(double &frameStartMS);

		void LoadConfig();
		void SaveConfig();
		bool CreateWindow();
		void RunGameLoop();

		void OnPaused();
		void OnUnpaused();

		friend class LoadLevelCommand;
	};
}
