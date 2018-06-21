#include "stdafx.h"
#include <chrono>
#include <thread>
#include "Engine.h"
#include "EngineCommands.h"
#include "World.h"
#include "Logger/Logger.h"
#include "Logger/FileLogger.h"
#include "Utils/PropRW.h"
#include "Config/EngineConfig.h"
#include "GameClock.h"
#include "Input/InputHandler.h"

#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/WindowController.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "SFMLInterface/Rendering/RenderParams.h"

#include "Entities/Actor.h"
#include "Levels/Level.h"
#include "Levels/LevelManager.h"

namespace Game {

	Engine::Ptr Engine::Create() {
		// std::make_unique requires public constructor and destructor access
		// So using a temporary struct
		struct unique_enabler : public Engine {};
		return std::make_unique<unique_enabler>();
	}

	Engine::Engine() : Object("Engine") {
		Logger::Log(*this, "Constructing Engine");
		LoadConfig();
		try {
			world = std::make_unique<World>(config->GetScreenSize(), config->GetWorldSize());
			assetManager = std::make_unique<AssetManager>();
			levelManager = std::make_unique<LevelManager>(*this);
			inputHandler = std::make_unique<InputHandler>();
			Logger::Log(*this, "Engine constructed and ready to Run()", Logger::Severity::Debug);
		}
		catch (std::bad_alloc e) {
			Logger::Log(*this, "Error constructing engine components!", Logger::Severity::Error);
			exitCode = ExitCode::InitError;
		}
	}

	Engine::~Engine() {
		levelManager = nullptr;
		assetManager = nullptr;
		world = nullptr;
		inputHandler = nullptr;
		windowController = nullptr;
		SaveConfig();
		config = nullptr;
		
		Logger::Log(*this, "Engine destroyed");
	}

	int Engine::Run() {
		Logger::Log(*this, "Execution started");

		if (exitCode == ExitCode::OK) {
			if (CreateWindow()) {
				RunGameLoop();
			}
		}

		return (int)exitCode;
	}

	InputHandler & Engine::GetInputHandler() const {
		return *inputHandler;
	}

	const World& Engine::GetWorld() const {
		return *world;
	}

	AssetManager & Engine::GetAssetManager() const {
		return *assetManager;
	}

	void Engine::LoadLevel(int id) {
		commands.emplace_back(std::make_unique<LoadLevelCommand>(*this, id));
	}

	void Engine::PollInput() {
		windowController->PollInput();
		if (!isPaused && !windowController->IsWindowFocussed()) {
			OnPaused();
		}
		if (isPaused && windowController->IsWindowFocussed()) {
			OnUnpaused();
		}
		inputHandler->CaptureState(windowController->GetInputHandler().GetPressed());
	}

	void Engine::FixedTick(Fixed& lag) {
		int fixedTicks = 0;
		while (lag >= Consts::MS_PER_FIXED_TICK) {
			levelManager->GetActiveLevel().FixedTick();
			lag -= Consts::MS_PER_FIXED_TICK;
			if (++fixedTicks > Consts::MAX_FIXED_TICKS) {
				Logger::Log(*this, "Timeout during FixedTick(). Ignore if pausing rendering", Logger::Severity::Warning);
				break;
			}
		}
	}

	void Engine::Tick(Fixed deltaTime) {
		GameClock::Tick(deltaTime);
		inputHandler->FireInput();
		levelManager->GetActiveLevel().Tick(deltaTime);
	}

	void Engine::Render() {
		windowController->Clear();
		RenderParams params(*windowController);
		levelManager->GetActiveLevel().Render(params);
		windowController->Display();
	}

	void Engine::PostRender(double &frameStartMS) {
		for (const auto& command : commands) {
			command->Execute();
		}
		commands.clear();

		double sinceStartMS = (static_cast<double>(clock.GetCurrentMicroseconds()) / 1000.0f) - frameStartMS;
		Fixed minFrameTimeMS = Fixed(1000, Consts::MAX_FPS);
		Fixed residue = Fixed(minFrameTimeMS.GetDouble() - sinceStartMS);
		if (residue > 0) {
			Logger::Log(*this, "Sleeping game loop for: " + residue.ToString() + "ms", Logger::Severity::HOT);
			std::this_thread::sleep_for(std::chrono::milliseconds(residue.GetInt()));
		}
	}

	void Engine::LoadConfig() {
		config = std::make_unique<EngineConfig>();
		if (config->Load("config.ini")) {
			Logger::Log(*this, "Loaded config.ini successfully", Logger::Severity::Debug);
		}
		Logger::SetLogLevel(config->GetLogLevel());
	}

	void Engine::SaveConfig() {
		if (config->Save("config.ini")) {
			Logger::Log(*this, "config.ini saved successfully");
		}
		else {
			Logger::Log(*this, "Could not save config.ini!", Logger::Severity::Warning);
		}
	}

	bool Engine::CreateWindow() {
		Vector2 screenSize = config->GetScreenSize();
		std::string windowTitle = config->GetWindowTitle();
		Logger::Log(*this, "Initialising window to (" + screenSize.ToString() + ")");
		try {
			windowController = std::make_unique<WindowController>(screenSize.x.GetInt(), screenSize.y.GetInt(), windowTitle);
		}
		catch (std::bad_alloc e) {
			Logger::Log(*this, "Error allocating Window Controller!", Logger::Severity::Error);
			exitCode = ExitCode::AllocationError;
			return false;
		}
		clock.Restart();
		return true;
	}

	void Engine::RunGameLoop() {
		if (!levelManager->LoadLevel(0)) {
			Logger::Log(*this, "Could not load level 0!", Logger::Severity::Error);
			exitCode = ExitCode::ExecutionError;
			return;
		}

		double previous = static_cast<double>(clock.GetCurrentMicroseconds()) * 0.001f;
		Fixed deltaTime = 0;
		Fixed lag = 0;
		while (windowController->IsWindowOpen()) {
			PollInput();
			double current = static_cast<double>(clock.GetCurrentMicroseconds()) * 0.001f;
			deltaTime = Fixed(current - previous);
			previous = current;
			
			if (!isPaused) {
				lag += deltaTime;
				FixedTick(lag);
				Tick(deltaTime);
				Render();
				PostRender(current);
			}
		}
	}

	void Engine::OnPaused() {
		isPaused = true;
		Logger::Log(*this, "Game paused");
	}

	void Engine::OnUnpaused() {
		isPaused = false;
		Logger::Log(*this, "Game unpaused");
	}
}
