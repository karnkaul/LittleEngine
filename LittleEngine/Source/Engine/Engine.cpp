#include "le_stdafx.h"
#include <chrono>
#include <thread>
#include "Engine.h"
#include "EngineCommands.h"
#include "World.h"
#include "Logger/Logger.h"
#include "Config/EngineConfig.h"
#include "GameClock.h"
#include "Input/InputHandler.h"
#include "Audio/AudioManager.h"
#include "Console/DebugConsole.h"
#include "Misc/Stopwatch.h"
#include "Levels/Level.h"
#include "Levels/LevelManager.h"
#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/WindowController.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;

	namespace {
		constexpr int MAX_FIXED_TICK_MS = 200;
		constexpr int MS_PER_FIXED_TICK = 6;
		constexpr float MIN_FRAME_TIME_MS = 1000 / Consts::MAX_FPS;

		inline double GetCurrentMicroseconds() {
			return static_cast<double>(SystemClock::GetCurrentMicroseconds()) * 0.001f;
		}
	}

	Engine::Ptr Engine::Create() {
		// std::make_unique requires public constructor and destructor access
		// So using a temporary struct
		struct unique_enabler : public Engine {};
		return std::make_unique<unique_enabler>();
	}

	Engine::Engine() : Object("Engine") {
		Logger::Log(*this, "Constructing Engine");
		try {
			/* Load Config */ {
				config = std::make_unique<EngineConfig>();
				if (config->Load("config.gdata")) {
					Logger::Log(*this, "Loaded config->ini successfully", Logger::Severity::Debug);
				}
				Logger::g_logLevel = config->GetLogLevel();
			}
			
			/* Instantiate entities */ {
				world = std::make_unique<World>(config->GetScreenSize());
				assetManager = std::make_unique<AssetManager>("Assets");
				audioManager = std::make_unique<AudioManager>(*this);
				levelManager = std::make_unique<LevelManager>(*this);
				inputHandler = std::make_unique<InputHandler>();
				Logger::Log(*this, "Engine constructed and ready to Run()", Logger::Severity::Debug);
			}

			Collider::DEBUG_BORDER_WIDTH = config->GetColliderBorderWidth();
		}
		catch (std::bad_alloc e) {
			Logger::Log(*this, "Error constructing engine components!", Logger::Severity::Error);
			exitCode = ExitCode::InitError;
		}	
	}

	Engine::~Engine() {
#if ENABLED(DEBUG_CONSOLE)
		DebugConsole::Cleanup();
#endif
		levelManager = nullptr;
		audioManager = nullptr;
		assetManager = nullptr;
		world = nullptr;
		inputHandler = nullptr;
		windowController = nullptr;
		if (config->Save("config.gdata")) {
			Logger::Log(*this, "config->ini saved successfully");
		}
		else {
			Logger::Log(*this, "Could not save config.gdata!", Logger::Severity::Warning);
		}
		
		Logger::Log(*this, "Engine destroyed");
		Logger::Cleanup();
	}

	ExitCode Engine::Run() {
		Logger::Log(*this, "Execution started");
		
		if (exitCode == ExitCode::OK) {
			if (CreateWindow()) {
				/* Load Level 0 */
				if (!levelManager->LoadLevel(BOOTLEVEL)) {
					Logger::Log(*this, "Could not load level 0!", Logger::Severity::Error);
					exitCode = ExitCode::ExecutionError;
					return exitCode;
				}

				/* Debugging */
#if ENABLED(DEBUG_CONSOLE)
				DebugConsole::Init(*this);
#endif

				/* Core Game Loop */
				double previous = GetCurrentMicroseconds();
				Fixed deltaTime = 0;
				Fixed lag = 0;
				while (windowController->IsWindowOpen() && !bIsQuitting) {
					/* Poll Window Events */ {
						windowController->PollEvents();
						if (!bIsPaused && !windowController->IsWindowFocussed()) {
							bIsPaused = true;
							Logger::Log(*this, "Game paused");
						}
						if (bIsPaused && windowController->IsWindowFocussed()) {
							bIsPaused = false;
							// Reset FixedTick time lag (account for clock not pausing)
							previous = GetCurrentMicroseconds();
							Logger::Log(*this, "Game unpaused");
						}
					}

					/* Process Frame */
					if (!bIsPaused) {
						inputHandler->ProcessInput(windowController->GetInput());

						double current = GetCurrentMicroseconds();
						deltaTime = Fixed(current - previous);
						previous = current;
						lag += deltaTime;

						/* Fixed Tick */ {
							int start = SystemClock::GetCurrentMilliseconds();
							while (lag >= MS_PER_FIXED_TICK) {
								levelManager->GetActiveLevel()->FixedTick();
								lag -= MS_PER_FIXED_TICK;
								if (SystemClock::GetCurrentMilliseconds() - start > MAX_FIXED_TICK_MS) {
									Logger::Log(*this, "Timeout during FixedTick(). Ignore if pausing rendering", Logger::Severity::Warning);
									break;
								}
							}
						}

						/* Tick */ {
							STOPWATCH_START("Tick");
							GameClock::Tick(deltaTime);
							inputHandler->FireInput();
							audioManager->Tick(deltaTime);
							levelManager->GetActiveLevel()->Tick(deltaTime);
							STOPWATCH_STOP();
						}

						/* Render */ {
							STOPWATCH_START("Render");
							RenderParams params(*windowController);
							levelManager->GetActiveLevel()->Render(params);
#if ENABLED(DEBUG_CONSOLE)
							DebugConsole::RenderConsole(*this, params, deltaTime);
#endif
							windowController->Draw();
							STOPWATCH_STOP();
						}
						
						/* Post Render: Commands */ {
							STOPWATCH_START("Post Render");
							for (const auto& command : commands) {
								(*command)();
							}
							commands.clear();
							STOPWATCH_STOP();
						}

						/* Post Render: Sleep */ {
							STOPWATCH_START("Sleep");
							float sinceStartMS = (static_cast<float>(SystemClock::GetCurrentMicroseconds()) * 0.001f) - current;
							auto residue = Fixed(MIN_FRAME_TIME_MS - sinceStartMS);
							if (residue > Fixed::Zero) {
								Logger::Log(*this, "Sleeping game loop for: " + residue.ToString() + "ms", Logger::Severity::HOT);
								std::this_thread::sleep_for(std::chrono::milliseconds(residue.ToInt()));
							}
							STOPWATCH_STOP();
						}
					}
				}
			}
		}

		return exitCode;
	}

	const LevelID Engine::GetActiveLevelID() const {
		return levelManager->GetActiveLevelID();
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

	AudioManager & Engine::GetAudioManager() const {
		return *audioManager;
	}

	void Engine::LoadLevel(const LevelID& levelID) {
		commands.emplace_back(std::make_unique<LoadLevelCommand>(*levelManager, levelID));
	}

	void Engine::Quit() {
		bIsQuitting = true;
	}

	bool Engine::CreateWindow() {
		Vector2 screenSize = config->GetScreenSize();
		std::string windowTitle = config->GetWindowTitle();
		Logger::Log(*this, "Initialising window to " + screenSize.ToString());
		try {
			windowController = std::make_unique<WindowController>(screenSize.x.ToInt(), screenSize.y.ToInt(), windowTitle);
		}
		catch (std::bad_alloc e) {
			Logger::Log(*this, "Error allocating Window Controller!", Logger::Severity::Error);
			exitCode = ExitCode::AllocationError;
			return false;
		}
		SystemClock::Restart();
		return true;
	}
}
