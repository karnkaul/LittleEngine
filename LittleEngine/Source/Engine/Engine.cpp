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

#include "SFMLInterface/Assets.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/WindowController.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "Misc/Stopwatch.h"
#include "Levels/Level.h"
#include "Levels/LevelManager.h"

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;

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
				if (config->Load("config.ini")) {
					Logger::Log(*this, "Loaded config.ini successfully", Logger::Severity::Debug);
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
		DebugConsole::Cleanup();
		levelManager = nullptr;
		audioManager = nullptr;
		assetManager = nullptr;
		world = nullptr;
		inputHandler = nullptr;
		windowController = nullptr;
		if (config->Save("config.ini")) {
			Logger::Log(*this, "config.ini saved successfully");
		}
		else {
			Logger::Log(*this, "Could not save config.ini!", Logger::Severity::Warning);
		}
		config = nullptr;
		
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
				DebugConsole::Init(*this);

				/* Core Game Loop */
				double previous = static_cast<double>(SystemClock::GetCurrentMicroseconds()) * 0.001f;
				double debugToggleTime = 0;
				Fixed deltaTime = 0;
				Fixed lag = 0;
				while (windowController->IsWindowOpen() && !bIsQuitting) {
					/* Poll Input */ {
						windowController->PollInput();
						if (!bIsPaused && !windowController->IsWindowFocussed()) {
							bIsPaused = true;
							Logger::Log(*this, "Game paused");
						}
						if (bIsPaused && windowController->IsWindowFocussed()) {
							bIsPaused = false;
							// Reset FixedTick time lag (account for clock not pausing)
							previous = static_cast<double>(SystemClock::GetCurrentMicroseconds()) * 0.001f;
							Logger::Log(*this, "Game unpaused");
						}
						
						// Debug Console and Input
						{
#if defined(DEBUG) || defined(_DEBUG)
							// Ignore consecutive Backticks for 200 ms
							const static double KEYPRESS_DELAY_MS = 200.0f;
							if ((previous - debugToggleTime) > KEYPRESS_DELAY_MS) {
								if (windowController->GetInput().IsKeyPressed(KeyCode::Backtick)) {
									DebugConsole::Activate(!DebugConsole::IsActive());
									debugToggleTime = previous;
								}
							}
#endif
							const Input& sfmlInput = windowController->GetInput();
							if (DebugConsole::IsActive()) {
								DebugConsole::UpdateInput(sfmlInput.GetRawSFMLInput());
							}
							else {
								inputHandler->CaptureState(sfmlInput.GetPressed());
								inputHandler->CaptureRawText(sfmlInput.GetRawSFMLInput().text);
							}
						}
					}

					/* Process Frame */
					if (!bIsPaused) {
						double current = static_cast<double>(SystemClock::GetCurrentMicroseconds()) * 0.001f;
						deltaTime = Fixed(current - previous);
						previous = current;
						lag += deltaTime;

						/* Fixed Tick */ {
							int fixedTicks = 0;
							while (lag >= Consts::MS_PER_FIXED_TICK) {
								levelManager->GetActiveLevel()->FixedTick();
								lag -= Consts::MS_PER_FIXED_TICK;
								if (++fixedTicks > Consts::MAX_FIXED_TICKS) {
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
							DebugConsole::RenderConsole(*this, params, deltaTime);
							windowController->Draw();
							STOPWATCH_STOP();
						}
						
						/* Post Render: Commands */
						STOPWATCH_START("Post Render");
						for (const auto& command : commands) {
							(*command)();
						}
						commands.clear();
						STOPWATCH_STOP();

						STOPWATCH_START("Sleep");
						/* Post Render: Sleep */
						double sinceStartMS = (static_cast<double>(SystemClock::GetCurrentMicroseconds()) / 1000.0f) - current;
						Fixed minFrameTimeMS = Fixed(1000, Consts::MAX_FPS);
						Fixed residue = Fixed(minFrameTimeMS.GetDouble() - sinceStartMS);
						if (residue > 0) {
							Logger::Log(*this, "Sleeping game loop for: " + residue.ToString() + "ms", Logger::Severity::HOT);
							std::this_thread::sleep_for(std::chrono::milliseconds(residue.GetInt()));
						}
						STOPWATCH_STOP();
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
			windowController = std::make_unique<WindowController>(screenSize.x.GetInt(), screenSize.y.GetInt(), windowTitle);
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
