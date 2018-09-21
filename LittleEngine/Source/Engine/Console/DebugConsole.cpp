#include "stdafx.h"
#include "DebugConsole.h"
#include "SFMLInterface/Rendering/RenderFactory.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "SFMLInterface/Rendering/TextRenderer.h"
#include "SFMLInterface/WindowController.h"
#include "SFMLInterface/Assets.h"
#include "Engine/World.h"
#include "Engine/Logger/Logger.h"
#include "SFML/Window/Event.hpp"

namespace LittleEngine { namespace DebugConsole {
	struct DebugConsoleRenderer {
		std::unique_ptr<RectangleRenderer> background = nullptr;
		std::unique_ptr<TextRenderer> logText = nullptr;
	};
	
	static DebugConsoleRenderer s_consoleRenderer;
	static bool s_isActive = false;
	const static RawTextInput* p_rawTextInput;
	static std::string s_liveString;
	
	void InitialiseComponents(const Engine* engine) {
		Vector2 bgSize(engine->GetWorld().GetScreenSize());
		if (s_consoleRenderer.background== nullptr) {
			s_consoleRenderer.background= RenderFactory::NewRectangle(bgSize);
			s_consoleRenderer.background->SetFillColour(Colour(100, 100, 100, 50));
			s_consoleRenderer.background->layer = LayerID::TOP;
		}
		if (s_consoleRenderer.logText == nullptr) {
			TextData textData(engine->GetAssetManager().GetDefaultFont(), "Test");
			s_consoleRenderer.logText = RenderFactory::NewText(textData);
			s_consoleRenderer.logText->SetColour(Colour::White);
			s_consoleRenderer.logText->SetSize(15);
		}
	}

	bool IsActive() {
		return s_isActive;
	}

	void Activate(bool setActive) {
		s_isActive = setActive;
	}

	void UpdateInput(const WindowController & windowController) {
		p_rawTextInput = &windowController.GetRawSFMLInput();
		if (!p_rawTextInput->text.empty()) {

		}
		switch (p_rawTextInput->special) {
		case RawTextInputType::Backspace:
			if (!s_liveString.empty()) {
				s_liveString = s_liveString.substr(0, s_liveString.size() - 1);
			}
			return;

		case RawTextInputType::Escape:
		case RawTextInputType::Enter:
			s_liveString.clear();
			return;
		}
		if (p_rawTextInput->text == "`") {
			return;
		}
		s_liveString += p_rawTextInput->text;
	}

	void RenderConsole(const Engine& engine, RenderParams& params) {
		if (s_isActive) {
			InitialiseComponents(&engine);
			params.screenPosition = engine.GetWorld().WorldToScreenPoint(Vector2::Zero);
			s_consoleRenderer.background->Render(params);

			params.screenPosition = engine.GetWorld().WorldToScreenPoint(Vector2(0, 300));
			s_consoleRenderer.logText->GetTextData().text = s_liveString;
			s_consoleRenderer.logText->Render(params);
		}
	}
	
	void Cleanup() {
		s_consoleRenderer.background= nullptr;
		s_consoleRenderer.logText = nullptr;
		s_isActive = false;
	}
} }
