#include "le_stdafx.h"
#include <list>
#include "DebugConsole.h"
#include "SFMLInterface/Rendering/RenderFactory.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "SFMLInterface/Rendering/TextRenderer.h"
#include "SFMLInterface/WindowController.h"
#include "SFMLInterface/Assets.h"
#include "Engine/World.h"
#include "Engine/Logger/Logger.h"
#include "SFML/Window/Event.hpp"
#include "Transform.h"
#include "LogLine.hpp"
#include "ConsoleCommands.h"
#include "Utils.h"

namespace LittleEngine { namespace DebugConsole {
	static bool s_isActive = false;
	static std::string s_liveString;
	static Fixed yOffsetMin = Fixed(-200);
	static Fixed yOffsetMax = Fixed::Zero;
	static Fixed yOffset = yOffsetMin;

	const static Colour LOG_BG_COLOUR = Colour(100, 100, 100, 50);

	const Colour LOG_TEXT_COLOUR = Colour(180, 180, 180);
	const Colour LOG_QUERY_COLOUR = Colour(120, 120, 120);
	const Colour LOG_WARNING_COLOUR = Colour(255, 255, 0);

	template<typename T>
	struct CircularList {
		int maxElements = -1;

		CircularList() {
			iter = items.begin();
		}

		void PushBack(const T& value, bool resetIter = true) {
			items.emplace_back(value);
			if (maxElements > 0 && items.size() > maxElements) {
				items.pop_front();
			}
			if (resetIter) {
				iter = items.begin();
			}
		}

		void PushFront(const T& value, bool resetIter = true) {
			items.emplace_front(value);
			if (maxElements > 0 && items.size() > maxElements) {
				items.pop_back();
			}
			if (resetIter) {
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
			bool overflown = false;
			if (iter == items.begin()) {
				iter = items.end();
				overflown = true;
			}
			--iter;
			return overflown;
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

	struct DebugConsoleLog {
		using Transform = GameUtils::Transform;

		const static int NUM_LOG_LINES = 10;
		Transform logT[NUM_LOG_LINES];

		DebugConsoleLog(TextData& baseData) {
			for (auto& l : logText) {
				l = RenderFactory::NewText(baseData);
				l->SetSize(15);
				l->layer = LayerID::TOP;
			}
		}

		void UpdateLog(const std::string& newLine, const Colour& lineColour) {
			logLines.emplace_front(newLine, lineColour);
			if (logLines.size() > NUM_LOG_LINES) {
				logLines.pop_back();
			}
			size_t index = 0;
			for (const LogLine& logLine : logLines) {
				logText[index]->GetTextData().text = logLine.text;
				logText[index]->SetColour(logLine.colour);
				++index;
			}
		}

		void Render(RenderParams& params) {
			for (size_t i = 0; i < NUM_LOG_LINES; ++i) {
				params.screenPosition = logT[i].Position();
				logText[i]->Render(params);
			}
		}

		std::unique_ptr<TextRenderer> logText[NUM_LOG_LINES] = { nullptr };
		std::list<LogLine> logLines;
	};

	struct DebugConsoleRenderer {
		using Transform = GameUtils::Transform;
		const Engine* engine = nullptr;

		void Log(const std::string& message, const Colour& colour) {
			log->UpdateLog(message, colour);
		}

		void Render(RenderParams& params, Fixed yOffset, bool drawCursor) {
			if (!engine) {
				return;
			}

			InitialiseComponents();
			rootT.localPosition.y = ROOT_Y + yOffset;

			params.screenPosition = rootT.Position();
			background->Render(params);

			log->Render(params);
			
			params.screenPosition = inputT.Position();
			std::string liveText(">" + s_liveString);
			if (drawCursor) {
				liveText += "_";
			}
			inputText->GetTextData().text = liveText;
			inputText->Render(params);

			params.screenPosition = separatorT.Position();
			separator->Render(params);
		}

	private:
		
		const static int LOG_LINE_HEIGHT = 4;

		Transform rootT;
		Transform inputT;
		Transform separatorT;
		std::unique_ptr<RectangleRenderer> background = nullptr;
		std::unique_ptr<RectangleRenderer> separator = nullptr;
		std::unique_ptr<DebugConsoleLog> log = nullptr;
		std::unique_ptr<TextRenderer> inputText = nullptr;
		Fixed ROOT_Y;

		void InitialiseComponents() {
			Vector2 bgSize(engine->GetWorld().GetScreenSize());
			bgSize.y *= Fixed::OneThird;
			if (log == nullptr) {
				TextData textData(engine->GetAssetManager().GetDefaultFont(), "");
				textData.hAlign = HAlign::Left;
				log = std::make_unique<DebugConsoleLog>(textData);

				inputText = RenderFactory::NewText(textData);
				inputText->SetColour(Colour::White);
				inputText->SetSize(16);
				inputText->layer = LayerID::TOP;
			}
			if (rootT.localPosition == Vector2::Zero) {
				rootT.localPosition = N2Screen(Vector2(Fixed::Zero, Fixed(3, 4)));
				ROOT_Y = rootT.localPosition.y;
				Fixed textX = Fixed(-95, 100);
				int textYNumerator = 52;
				inputT.localPosition = N2Screen(Vector2(textX, Fixed(textYNumerator, 100)));
				inputT.SetParent(rootT, false);
				textYNumerator += 2;
				separatorT.localPosition = N2Screen(Vector2(Fixed::Zero, Fixed(textYNumerator, 100)));
				textYNumerator += LOG_LINE_HEIGHT;
				separatorT.SetParent(rootT, false);
				for (auto& l : log->logT) {
					textYNumerator += LOG_LINE_HEIGHT;
					l.localPosition = N2Screen(Vector2(textX, Fixed(textYNumerator, 100)));
					l.SetParent(rootT, false);
				}
			}
			if (background == nullptr) {
				background = RenderFactory::NewRectangle(bgSize);
				background->SetFillColour(LOG_BG_COLOUR);
				background->layer = LayerID::TOP;
			}
			if (separator == nullptr) {
				Vector2 separatorSize(bgSize.x, Fixed::One);
				separator = RenderFactory::NewRectangle(separatorSize);
				separator->SetFillColour(LOG_TEXT_COLOUR);
				separator->layer = LayerID::TOP;
			}
		}

		Vector2 N2Screen(const Vector2 nPoint) {
			if (!engine) {
				return nPoint;
			}

			const World& world = engine->GetWorld();
			return world.WorldToScreenPoint(world.NormalisedToWorldPoint(nPoint, false));
		}
	};

	static std::unique_ptr<DebugConsoleRenderer> s_consoleRenderer;
	static CircularList<std::string> s_queryCache;
	static bool s_bCyclingQueries = false;

	void ProcessCommand() {
		std::vector<LogLine> logOutput = Commands::Execute(s_liveString);
		for (const auto& l : logOutput) {
			s_consoleRenderer->Log(l.text, l.colour);
		}
		s_queryCache.PushFront(s_liveString);
		s_liveString.clear();
	}
	
	void Init(Engine& engine) {
		Commands::Init(engine);
		s_consoleRenderer = std::make_unique<DebugConsoleRenderer>();
		s_consoleRenderer->engine = &engine;
		s_queryCache.maxElements = 10;
	}

	bool IsActive() {
		return s_isActive;
	}

	void Activate(bool setActive) {
		s_isActive = setActive;
	}

	void UpdateInput(const RawTextInput& rawTextInput) {
		if (rawTextInput.special != RawTextInputType::None && rawTextInput.special != RawTextInputType::Up && rawTextInput.special != RawTextInputType::Down) {
			s_bCyclingQueries = false;
			s_queryCache.ResetIter();
		}

		switch (rawTextInput.special) {
		case RawTextInputType::Backspace:
			if (!s_liveString.empty()) {
				s_liveString = s_liveString.substr(0, s_liveString.size() - 1);
			}
			return;

		case RawTextInputType::Tab:
		{
			if (!s_liveString.empty()) {
				std::vector<std::string> search = Commands::AutoComplete(s_liveString);
				if (search.empty()) {
					s_consoleRenderer->Log("Unrecognised query: \"" + s_liveString + "\"", LOG_WARNING_COLOUR);
				}
				else if (search.size() == 1) {
					s_liveString = search[0] + " ";
				}
				else {
					std::string logOutput;
					for (const auto& s : search) {
						logOutput += ("\t" + s);
					}
					s_consoleRenderer->Log(logOutput, LOG_TEXT_COLOUR);
				}
			}
		}
			return;
		
		case RawTextInputType::Enter:
			ProcessCommand();
			return;

		case RawTextInputType::Up:
			if (!s_bCyclingQueries) {
				if (!s_liveString.empty()) {
					s_queryCache.PushFront(s_liveString);
					s_queryCache.Increment();
				}
				s_bCyclingQueries = true;
			}
			else {
				s_queryCache.Increment();
			}
			s_liveString = s_queryCache.Get();
			return;

		case RawTextInputType::Down:
			if (!s_bCyclingQueries) {
				if (!s_liveString.empty()) {
					s_queryCache.PushFront(s_liveString);
				}
				s_bCyclingQueries = true;
			}
			s_queryCache.Decrement();
			s_liveString = s_queryCache.Get();
			return;

		case RawTextInputType::Escape:
			s_liveString.clear();
			return;
		}
		if (rawTextInput.text == "`") {
			return;
		}
		s_liveString += rawTextInput.text;
	}

	void RenderConsole(const Engine& engine, RenderParams& params, Fixed deltaTime) {
		if (!s_consoleRenderer) {
			return;
		}
		const Fixed ANIM_SCALE = Fixed::Two;
		static double elapsed = 0.0f;
		elapsed += deltaTime.GetDouble();
		if (s_isActive) {
			if (yOffset < yOffsetMax) {
				yOffset += (deltaTime * ANIM_SCALE);
			}
		}
		else {
			if (yOffset > yOffsetMin) {
				yOffset -= (deltaTime * ANIM_SCALE);
			}
		}
		yOffset = Maths::Clamp(yOffset, yOffsetMin, yOffsetMax);
		if (yOffset > yOffsetMin) {
			int doubleCadence = static_cast<int>(elapsed / 300.0f);
			s_consoleRenderer->Render(params, yOffset, doubleCadence % 2 == 0);
		}
	}
	
	void Cleanup() {
		s_consoleRenderer = nullptr;
		s_isActive = false;
	}
} }
