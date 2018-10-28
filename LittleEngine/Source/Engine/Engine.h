#pragma once
#include <vector>
#include "Object.h"
#include "Fixed.h"
#include "Delegate.hpp"
#include "SFMLInterface/SystemClock.h"
#include "Events/EventManager.h"
#include "Levels/LevelManager.h"

namespace Consts {
	constexpr int MAX_FIXED_TICKS = 20;
	constexpr int MS_PER_FIXED_TICK = 6;
	constexpr int MAX_FPS = 125;
}

namespace LittleEngine {
	class Actor;
	class World;
	class EngineConfig;
	class AssetManager;
	class AudioManager;
	class Level;
	class WindowController;
	class EngineCommand;
	class InputHandler;
	using LevelID = int;

	enum class ExitCode { 
		OK,
		InitError,
		DependencyError,
		AllocationError,
		ExecutionError,
		CleanupError
	};
	
	// \brief Core Engine class
	class Engine : Object {
	public:
		using Ptr = std::unique_ptr<Engine>;
		
		// Disable Object's intended constructor
		Engine(const std::string&) = delete;
		~Engine();
		// Copy constructor and copy assignment operators
		// will be auto-deleted due to unique_ptr members
		
		// (Only) Factory for unique_ptr<Engine>
		static Ptr Create();
		
		template<typename T>
		LevelID CreateLevel() {
			LevelID id = levelManager->template CreateLevel<T>();
			return id;
		}

		// \brief Creates window and attempts to load Level 0
		// \returns exit code
		ExitCode Run();

		const LevelID GetActiveLevelID() const;
		InputHandler& GetInputHandler() const;
		const World& GetWorld() const;
		AssetManager& GetAssetManager() const;
		AudioManager& GetAudioManager() const;
		void LoadLevel(const LevelID& levelID);
		void Quit();

	private:
		std::unique_ptr<AssetManager> assetManager;
		std::unique_ptr<LevelManager> levelManager;
		std::unique_ptr<AudioManager> audioManager;
		std::unique_ptr<EngineConfig> config;
		std::unique_ptr<World> world;
		std::unique_ptr<WindowController> windowController;
		std::unique_ptr<InputHandler> inputHandler;
		std::vector<std::unique_ptr<EngineCommand>> commands;
		ExitCode exitCode = ExitCode::OK;
		bool isPaused = false;
		bool isQuitting = false;

		Engine();

		bool CreateWindow();
	};
}
