#include "stdafx.h"
#include "BootLevel.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"
#include "Components/RenderComponent.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/RenderFactory.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "SFMLInterface/Rendering/TextRenderer.h"
#include "SFMLInterface/Assets.h"

namespace Game {
	BootLevel::BootLevel(Engine& engine) : Level("BootLevel", engine) {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);
		LoadAssets();
		
		Vector2 worldY = this->engine->GetWorld().GetWorldBoundsY();
		Fixed logoY = worldY.x + 200;
		logo = NewActor("Logo");
		auto& renderer = logo->AddComponent<RenderComponent>();
		logoRenderer = &renderer.SetTextRenderer("... Press Enter to Start ...");
		logo->GetTransform()->localPosition = Vector2(0, logoY);
	}

	void BootLevel::Tick(Fixed deltaTime) {
		Level::Tick(deltaTime);
		if (GetInput().IsKeyPressed(KeyCode::Enter)) {
			engine->LoadLevel(1);
		}

		Fixed seconds = Fixed(static_cast<int>(LevelTimeMilliSeconds()), 1000);
		Fixed speed = 2;
		Fixed alpha = (seconds * speed).Sin().Abs() * 255;
		Colour c = logoRenderer->GetTextData().fillColour;
		logoRenderer->GetTextData().fillColour = Colour(c.r, c.g, c.g, alpha);
	}

	void BootLevel::LoadAssets() {
		Logger::Log(*this, "Loading Assets...", Logger::Severity::Debug);
		engine->GetAssetManager().LoadAllTextures({ "Assets/Ship.png" });
	}
}
