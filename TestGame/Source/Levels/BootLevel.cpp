#include "stdafx.h"
#include "BootLevel.h"
#include "SFMLInterface/All.h"
#include "Components/All.h"

namespace LittleEngine {
	void BootLevel::OnActivated() {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);
		
		//Vector2 lowerBound = this->engine->GetWorld().GetScreenBounds().lower;
		if (Actor* logo = SpawnActor<Actor>("Logo", true)) {
			m_logoID = logo->GetActorID();
			auto renderer = logo->AddComponent<RenderComponent>();
			m_pLogoRenderable = &renderer->SetTextRenderable("... Press Enter to Start ...");
			logo->SetNormalisedPosition(Vector2(0, Fixed(-0.66f)));
		}

		RegisterScopedInput(GameInput::Enter, std::bind(&BootLevel::OnLoadNextLevel, this), OnKey::Released);
		RegisterScopedInput(GameInput::Return, std::bind(&Level::Quit, this), OnKey::Released);

		//this->engine->GetAudioManager().PlayMusic("TestMusic.ogg");
	}

	void BootLevel::Tick(Fixed deltaTime) {
		// Fade logo
		if (IsAlive(m_logoID)) {
			Fixed seconds = Fixed(static_cast<int>(LevelTimeMilliSeconds()), 1000);
			Fixed speed = 2;
			Fixed alpha = (seconds * speed).Sin().Abs() * 255;
			Colour c = m_pLogoRenderable->GetTextData().GetFillColour();
			m_pLogoRenderable->GetTextData().SetFillColour(Colour(c.r, c.g, c.g, UByte(alpha.ToInt())));
		}

		// Tick all actors etc
		Level::Tick(deltaTime);
	}

	void BootLevel::OnLoadNextLevel() {
		LoadLevel(GetActiveLevelID() + 1);
	}
}
