#include "le_stdafx.h"
#include <chrono>
#include <thread>
#include "Engine.h"
#include "EngineCommands.h"
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
#include "SFMLInterface/Graphics.h"
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;

	namespace {
		constexpr int MAX_FIXED_TICK_MS = 200;
		constexpr int MS_PER_FIXED_TICK = 6;
		constexpr float MIN_FRAME_TIME_MS = 1000 / Consts::MAX_FPS;

		inline double GetCurrentMilliseconds() {
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
				m_uConfig = std::make_unique<EngineConfig>();
				if (m_uConfig->Load("config.gdata")) {
					Logger::Log(*this, "Loaded config->ini successfully", Logger::Severity::Debug);
				}
				Logger::g_logLevel = m_uConfig->GetLogLevel();
			}
			
			/* Instantiate entities */ {
				m_uAssetManager = std::make_unique<AssetManager>("Assets");
				m_uAudioManager = std::make_unique<AudioManager>(*this);
				m_uLevelManager = std::make_unique<LevelManager>(*this);
				m_uInputHandler = std::make_unique<InputHandler>();
				Logger::Log(*this, "Engine constructed and ready to Run()", Logger::Severity::Debug);
			}

			Collider::DEBUG_BORDER_WIDTH = m_uConfig->GetColliderBorderWidth();
		}
		catch (std::bad_alloc e) {
			Logger::Log(*this, "Error constructing engine components!", Logger::Severity::Error);
			m_exitCode = ExitCode::InitError;
		}	
	}

	Engine::~Engine() {
#if ENABLED(DEBUG_CONSOLE)
		DebugConsole::Cleanup();
#endif
		m_uLevelManager = nullptr;
		m_uAudioManager = nullptr;
		m_uAssetManager = nullptr;
		m_uInputHandler = nullptr;
		Graphics::DestroyInstance();
		m_pGraphics = nullptr;
		if (m_uConfig->Save("config.gdata")) {
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
		
		if (m_exitCode == ExitCode::OK) {
			if (CreateWindow()) {
				/* Load Level 0 */
				if (!m_uLevelManager->LoadLevel(BOOTLEVEL)) {
					Logger::Log(*this, "Could not load level 0!", Logger::Severity::Error);
					m_exitCode = ExitCode::ExecutionError;
					return m_exitCode;
				}

				/* Debugging */
#if ENABLED(DEBUG_CONSOLE)
				DebugConsole::Init(*this);
#endif

				/* Core Game Loop */
				double previous = GetCurrentMilliseconds();
				Fixed deltaMS = 0;
				Fixed lag = 0;
				while (m_pGraphics->IsWindowOpen() && !m_bIsQuitting) {
					/* Poll Window Events */ {
						m_pGraphics->PollEvents();
						if (!m_bIsPaused && !m_pGraphics->IsWindowFocussed()) {
							m_bIsPaused = true;
							Logger::Log(*this, "Game paused");
						}
						if (m_bIsPaused && m_pGraphics->IsWindowFocussed()) {
							m_bIsPaused = false;
							// Reset FixedTick time lag (account for clock not pausing)
							previous = GetCurrentMilliseconds();
							Logger::Log(*this, "Game unpaused");
						}
					}

					/* Process Frame */
					if (!m_bIsPaused) {
						m_uInputHandler->ProcessInput(m_pGraphics->GetInput());

						double current = GetCurrentMilliseconds();
						deltaMS = Fixed(current - previous);
						previous = current;
						lag += deltaMS;

						/* Fixed Tick */ {
							int start = SystemClock::GetCurrentMilliseconds();
							while (lag >= MS_PER_FIXED_TICK) {
								m_uLevelManager->GetActiveLevel()->FixedTick();
								lag -= MS_PER_FIXED_TICK;
								if (SystemClock::GetCurrentMilliseconds() - start > MAX_FIXED_TICK_MS) {
									Logger::Log(*this, "Timeout during FixedTick(). Ignore if pausing rendering", Logger::Severity::Warning);
									break;
								}
							}
						}

						/* Tick */ {
							STOPWATCH_START("Tick");
							GameClock::Tick(deltaMS);
							m_uInputHandler->FireInput();
							m_uAudioManager->Tick(deltaMS);
							m_uLevelManager->GetActiveLevel()->Tick(deltaMS);
							STOPWATCH_STOP();
						}

						/* Render */ {
							STOPWATCH_START("Render");
							m_uLevelManager->GetActiveLevel()->Render();
#if ENABLED(DEBUG_CONSOLE)
							DebugConsole::RenderConsole(deltaMS);
#endif
							m_pGraphics->Draw();
							STOPWATCH_STOP();
						}
						
						/* Post Render: Commands */ {
							STOPWATCH_START("Post Render");
							for (const auto& command : m_commands) {
								(*command)();
							}
							m_commands.clear();
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

		return m_exitCode;
	}

	LevelID Engine::GetActiveLevelID() const {
		return m_uLevelManager->GetActiveLevelID();
	}

	InputHandler & Engine::GetInputHandler() const {
		return *m_uInputHandler;
	}

	AssetManager & Engine::GetAssetManager() const {
		return *m_uAssetManager;
	}

	AudioManager & Engine::GetAudioManager() const {
		return *m_uAudioManager;
	}

	void Engine::LoadLevel(const LevelID& levelID) {
		m_commands.emplace_back(std::make_unique<LoadLevelCommand>(*m_uLevelManager, levelID));
	}

	void Engine::Quit() {
		m_bIsQuitting = true;
	}

	bool Engine::CreateWindow() {
		Vector2 screenSize = m_uConfig->GetScreenSize();
		std::string windowTitle = m_uConfig->GetWindowTitle();
		Logger::Log(*this, "Initialising window to " + screenSize.ToString());
		try {
			GraphicsData data;
			data.screenWidth = screenSize.x.ToInt();
			data.screenHeight = screenSize.y.ToInt();
			data.windowTitle = windowTitle;
			data.viewSize = m_uConfig->GetViewSize();
			m_pGraphics = &Graphics::CreateInstance(data);
		}
		catch (std::bad_alloc&) {
			Logger::Log(*this, "Error instantiating Graphics!", Logger::Severity::Error);
			m_exitCode = ExitCode::AllocationError;
			return false;
		}
		catch (GraphicsException& e) {
			Logger::Log(*this, e.errMsg, Logger::Severity::Error);
			m_exitCode = ExitCode::InitError;
			return false;
		}
		SystemClock::Restart();
		return true;
	}
}
