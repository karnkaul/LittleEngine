#include "le_stdafx.h"
#include "DebugConsole.h"
#if ENABLED(DEBUG_CONSOLE)
#include <list>
#include "SFMLInterface/Rendering/ShapeRenderable.h"
#include "SFMLInterface/Rendering/TextRenderable.h"
#include "SFMLInterface/Graphics.h"
#include "SFMLInterface/Assets.h"
#include "Engine/Logger/Logger.h"
#include "Engine/CoreGame.hpp"
#include "SFML/Window/Event.hpp"
#include "LogLine.hpp"
#include "ConsoleCommands.h"
#include "ConsoleProfiler.h"
#include "Utils.h"

namespace LittleEngine { namespace DebugConsole {
	const Colour LOG_BG_COLOUR = Colour(100, 100, 100, 50);
	const Colour LOG_TEXT_COLOUR = Colour(180, 180, 180);
	const Colour LOG_QUERY_COLOUR = Colour(120, 120, 120);
	const Colour LOG_WARNING_COLOUR = Colour(255, 255, 0);

	template<typename T>
	struct CircularList {
		int maxElements = -1;

		CircularList() {
			iter = items.begin();
		}

		void PushBack(const T& value, bool bResetIter = true) {
			items.emplace_back(value);
			if (maxElements > 0 && items.size() > maxElements) {
				items.pop_front();
			}
			if (bResetIter) {
				iter = items.begin();
			}
		}

		void PushFront(const T& value, bool bResetIter = true) {
			items.emplace_front(value);
			if (maxElements > 0 && items.size() > static_cast<size_t>(maxElements)) {
				items.pop_back();
			}
			if (bResetIter) {
				iter = items.begin();
			}
		}

		void ResetIter() {
			iter = items.begin();
		}

		bool IsEmpty() const {
			return items.empty();
		}

		// Returns false if overflown (and resets to head)
		bool Increment() {
			if (items.empty()) {
				return false;
			}
			++iter;
			if (iter == items.end()) {
				iter = items.begin();
				return false;
			}
			return true;
		}

		// Returns false if overflown (and resets to tail)
		bool Decrement() {
			if (items.empty()) {
				return false;
			}
			bool bOverflown = false;
			if (iter == items.begin()) {
				iter = items.end();
				bOverflown = true;
			}
			--iter;
			return bOverflown;
		}

		void Clear() {
			items.clear();
			iter = items.begin();
		}

		const T& Get() const {
			return *iter;
		}

	private:
		typename std::list<T>::const_iterator iter;
		std::list<T> items;
	};

	namespace {
		bool bIsActive = false;
		std::string liveString;
		Fixed yOffsetMin = Fixed(-200);
		Fixed yOffsetMax = Fixed::Zero;
		Fixed yOffset = yOffsetMin;

		struct DebugConsoleLog {
			static const int NUM_LOG_LINES = 10;
			Transform logT[NUM_LOG_LINES];

			DebugConsoleLog(TextData& baseData) {
				for (auto& l : logText) {
					l = std::make_unique<TextRenderable>(baseData);
					l->SetSize(15);
					l->m_layer = LayerID::TOP;
				}
			}

			void UpdateLog(const std::string& newLine, const Colour& lineColour) {
				logLines.emplace_front(newLine, lineColour);
				if (logLines.size() > NUM_LOG_LINES) {
					logLines.pop_back();
				}
				size_t index = 0;
				for (const LogLine& logLine : logLines) {
					logText[index]->GetTextData().SetText(logLine.text);
					logText[index]->SetColour(logLine.colour);
					++index;
				}
			}

			void Render(RenderParams& params) {
				for (size_t i = 0; i < NUM_LOG_LINES; ++i) {
					params.worldPosition = logT[i].Position();
					logText[i]->Render(params);
				}
			}

			std::unique_ptr<TextRenderable> logText[NUM_LOG_LINES] = { nullptr };
			std::list<LogLine> logLines;
		};

		struct DebugConsoleRenderer {
			const Engine* pEngine = nullptr;

			void Log(const std::string& message, const Colour& colour) {
				log->UpdateLog(message, colour);
			}

			void Render(Fixed yOffset, bool bDrawCursor) {
				if (!pEngine) return;

				InitialiseComponents();
				rootT.localPosition.y = ROOT_Y - yOffset;
				
				RenderParams params;
				params.worldPosition = rootT.Position();
				background->Render(params);

				log->Render(params);
			
				params.worldPosition = inputT.Position();
				std::string liveText(">" + liveString);
				if (bDrawCursor) {
					liveText += "_";
				}
				inputText->GetTextData().SetText(liveText);
				inputText->Render(params);

				params.worldPosition = separatorT.Position();
				separator->Render(params);
			}

		private:
		
			static const int LOG_LINE_HEIGHT = 4;

			Transform rootT;
			Transform inputT;
			Transform separatorT;
			std::unique_ptr<RectangleRenderable> background = nullptr;
			std::unique_ptr<RectangleRenderable> separator = nullptr;
			std::unique_ptr<DebugConsoleLog> log = nullptr;
			std::unique_ptr<TextRenderable> inputText = nullptr;
			Fixed ROOT_Y;

			void InitialiseComponents() {
				Vector2 bgSize(Graphics::GetGameViewSize());
				bgSize.y *= Fixed::OneThird;
				if (log == nullptr) {
					TextData textData(*pEngine->GetAssetManager().GetDefaultFont(), "");
					textData.SetHAlign(HAlign::Left);
					log = std::make_unique<DebugConsoleLog>(textData);

					inputText = std::make_unique<TextRenderable>(textData);
					inputText->SetColour(Colour::White);
					inputText->SetSize(16);
					inputText->m_layer = LayerID::TOP;
				}
				if (rootT.localPosition == Vector2::Zero) {
					rootT.localPosition = Graphics::NToWorld(Vector2(Fixed::Zero, Fixed(3, 4)), false);
					ROOT_Y = rootT.localPosition.y;
					Fixed textX = Fixed(-95, 100);
					int textYNumerator = 52;
					inputT.localPosition = Graphics::NToWorld(Vector2(textX, Fixed(textYNumerator, 100)), false);
					inputT.SetParent(rootT, false);
					textYNumerator += 2;
					separatorT.localPosition = Graphics::NToWorld(Vector2(Fixed::Zero, Fixed(textYNumerator, 100)), false);
					textYNumerator += LOG_LINE_HEIGHT;
					separatorT.SetParent(rootT, false);
					for (auto& l : log->logT) {
						textYNumerator += LOG_LINE_HEIGHT;
						l.localPosition = Graphics::NToWorld(Vector2(textX, Fixed(textYNumerator, 100)), false);
						l.SetParent(rootT, false);
					}
				}
				if (background == nullptr) {
					background = std::make_unique<RectangleRenderable>(bgSize);
					background->SetFillColour(LOG_BG_COLOUR);
					background->m_layer = LayerID::TOP;
				}
				if (separator == nullptr) {
					Vector2 separatorSize(bgSize.x, Fixed::One);
					separator = std::make_unique<RectangleRenderable>(separatorSize);
					separator->SetFillColour(LOG_TEXT_COLOUR);
					separator->m_layer = LayerID::TOP;
				}
			}
		};

		std::unique_ptr<DebugConsoleRenderer> consoleRenderer;
		CircularList<std::string> queryCache;
		bool bCyclingQueries = false;

		void ProcessCommand() {
			std::vector<LogLine> logOutput = Commands::Execute(liveString);
			for (const auto& l : logOutput) {
				consoleRenderer->Log(l.text, l.colour);
			}
			queryCache.PushFront(liveString);
			liveString.clear();
		}
	}

	void Init(Engine& engine) {
		Commands::Init(engine);
#if ENABLED(DEBUG_PROFILER)
		Profiler::Init(engine);
#endif
		consoleRenderer = std::make_unique<DebugConsoleRenderer>();
		consoleRenderer->pEngine = &engine;
		queryCache.maxElements = 10;
	}

	bool IsActive() {
		return bIsActive;
	}

	void Activate(bool bSetActive) {
#if defined(DEBUG) || defined(_DEBUG)
		bIsActive = bSetActive;
#endif
	}

	void UpdateInput(const RawTextInput& rawTextInput) {
		static std::string previousLogText;

		if (rawTextInput.special != RawTextInputType::None && rawTextInput.special != RawTextInputType::Up && rawTextInput.special != RawTextInputType::Down) {
			bCyclingQueries = false;
			queryCache.ResetIter();
		}

		switch (rawTextInput.special) {
		case RawTextInputType::Backspace:
			if (!liveString.empty()) {
				liveString = liveString.substr(0, liveString.size() - 1);
			}
			return;

		case RawTextInputType::Tab:
		{
			if (!liveString.empty()) {
				Commands::AutoCompleteResults search = Commands::AutoComplete(liveString);
				if (search.queries.empty()) {
					consoleRenderer->Log("Unrecognised query: \"" + liveString + "\"", LOG_WARNING_COLOUR);
					return;
				}
				
				if (search.queries.size() == 1) {
					liveString = search.queries[0] + " ";
				}
				
				std::string logOutput;
				if (!search.params.empty()) {
					for (const auto& params : search.params) {
						logOutput += ("\t" + params);
					}
				}
				else {
					for (const auto& query : search.queries) {
						logOutput += ("\t" + query);
					}
				}
				if (!logOutput.empty() && logOutput != previousLogText) {
					previousLogText = logOutput;
					consoleRenderer->Log(logOutput, LOG_TEXT_COLOUR);
				}
			}
		}
			return;
		
		case RawTextInputType::Enter:
			if (!liveString.empty()) ProcessCommand();
			return;

		case RawTextInputType::Up:
			if (queryCache.IsEmpty()) return;
			if (!bCyclingQueries) {
				if (!liveString.empty()) {
					queryCache.PushFront(liveString);
					queryCache.Increment();
				}
				bCyclingQueries = true;
			}
			else {
				queryCache.Increment();
			}
			liveString = queryCache.Get();
			return;

		case RawTextInputType::Down:
			if (queryCache.IsEmpty()) return;
			if (!bCyclingQueries) {
				if (!liveString.empty()) {
					queryCache.PushFront(liveString);
				}
				bCyclingQueries = true;
			}
			queryCache.Decrement();
			liveString = queryCache.Get();
			return;

		case RawTextInputType::Escape:
			liveString.clear();
			return;
		
		default:
			break;
		}
		if (rawTextInput.text != "`") liveString += rawTextInput.text;
	}

	void RenderConsole(const Fixed& deltaMS) {
		if (!consoleRenderer) {
			return;
		}
		
		const Fixed ANIM_SCALE = Fixed::Two;
		static double elapsed = 0.0f;
		elapsed += deltaMS.ToDouble();
		if (bIsActive) {
			if (yOffset < yOffsetMax) {
				yOffset += (deltaMS * ANIM_SCALE);
			}
		}
		else {
			if (yOffset > yOffsetMin) {
				yOffset -= (deltaMS * ANIM_SCALE);
			}
		}
		yOffset = Maths::Clamp(yOffset, yOffsetMin, yOffsetMax);
		if (yOffset > yOffsetMin) {
			int doubleCadence = static_cast<int>(elapsed / 300.0f);
			consoleRenderer->Render(yOffset, doubleCadence % 2 == 0);
		}

#if ENABLED(DEBUG_PROFILER)
		Profiler::Tick(deltaMS);
		Profiler::Render();
#endif
	}
	
	void Cleanup() {
#if ENABLED(DEBUG_PROFILER)
		Profiler::Cleanup();
#endif
		consoleRenderer = nullptr;
		bIsActive = false;
	}
} }
#endif
