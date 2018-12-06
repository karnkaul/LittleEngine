#pragma once
#include <vector>
#include "Object.h"
#include "Fixed.h"
#include "Delegate.hpp"
#include "CoreGame.hpp"
#include "SFMLInterface/SystemClock.h"
#include "Events/EventManager.h"
#include "Levels/LevelManager.h"

namespace Consts {
	constexpr int MAX_FPS = 200;
}

namespace LittleEngine {
	using LevelID = int;

	enum class ExitCode { 
		OK					= 0,
		InitError			= 101,
		DependencyError		= 102,
		AllocationError		= 201,
		ExecutionError		= 301,
		CleanupError		= 401,
	};
	
	// \brief Core Engine class
	class Engine : Object {
	public:
		using Ptr = std::unique_ptr<Engine>;

	private:
		template<typename T>
		using UPtr = std::unique_ptr<T>;

		class Graphics* m_pGraphics;
		UPtr<LevelManager> m_uLevelManager;
		UPtr<class AssetManager> m_uAssetManager;
		UPtr<class AudioManager> m_uAudioManager;
		UPtr<class InputHandler> m_uInputHandler;
		UPtr<class EngineConfig> m_uConfig;
		std::vector<UPtr<class EngineCommand>> m_commands;
		ExitCode m_exitCode = ExitCode::OK;
		bool m_bIsPaused = false;
		bool m_bIsQuitting = false;

	public:
		// Disable Object's intended constructor
		Engine(const std::string&) = delete;
		~Engine();
		// Copy constructor and copy assignment operators
		// will be auto-deleted due to unique_ptr members
		
		// (Only) Factory for unique_ptr<Engine>
		static Ptr Create();
		
		template<typename T>
		LevelID CreateLevel() {
			LevelID id = m_uLevelManager->template CreateLevel<T>();
			return id;
		}

		// \brief Creates window and attempts to load Level 0
		// \returns exit code
		ExitCode Run();

		const LevelID GetActiveLevelID() const;
		InputHandler& GetInputHandler() const;
		AssetManager& GetAssetManager() const;
		AudioManager& GetAudioManager() const;
		void LoadLevel(const LevelID& levelID);
		void Quit();

	private:
		Engine();

		bool CreateWindow();
	};
}
