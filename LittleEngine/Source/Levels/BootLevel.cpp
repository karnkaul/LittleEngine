#include "stdafx.h"
#include "BootLevel.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Logger/Logger.h"
#include "Entities/Actor.h"
#include "Components/RenderComponent.h"
#include "SFMLInterface/Rendering/Renderer.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/RenderFactory.h"
#include "SFMLInterface/Rendering/ShapeRenderer.h"
#include "SFMLInterface/Rendering/TextRenderer.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	BootLevel::BootLevel(Engine& engine) : Level("BootLevel", engine) {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);
		static bool _assetsLoaded = false;
		if (!_assetsLoaded) {
			LoadAssets();
			_assetsLoaded = true;
		}
		
		Vector2 lowerBound = this->engine->GetWorld().GetScreenBounds().lower;
		_logo = SpawnActor<Actor>("Logo");
		std::shared_ptr<Actor> logo = nullptr;
		if ((logo = _logo.lock()) != nullptr) {
			auto renderer = logo->AddComponent<RenderComponent>();
			logoRenderer = &renderer->SetTextRenderer("... Press Enter to Start ...");
			logo->SetNormalisedPosition(Vector2(0, Fixed(-0.66f)));
		}

		inputTokens.push_back(GetInputHandler().Register(GameInput::Enter, std::bind(&BootLevel::OnLoadNextLevel, this), OnKey::Released));
		inputTokens.push_back(GetInputHandler().Register(GameInput::Return, std::bind(&BootLevel::OnQuit, this), OnKey::Released));
		//this->engine->GetAudioManager().PlayMusic("TestMusic.ogg");
	}

	void BootLevel::Tick(Fixed deltaTime) {
		// Fade logo
		Fixed seconds = Fixed(static_cast<int>(LevelTimeMilliSeconds()), 1000);
		Fixed speed = 2;
		Fixed alpha = (seconds * speed).Sin().Abs() * 255;
		Colour c = logoRenderer->GetTextData().fillColour;
		logoRenderer->GetTextData().fillColour = Colour(c.r, c.g, c.g, alpha);

		// Tick all actors etc
		Level::Tick(deltaTime);
	}

	void BootLevel::LoadAssets() {
		Logger::Log(*this, "Loading Assets...", Logger::Severity::Debug);
		AssetManager& assetManager = engine->GetAssetManager();
		assetManager.Load<TextureAsset>({ "Ship.png" });
		assetManager.Load<SoundAsset>({ "TestSound.wav", "TestSound_b.wav" });
		assetManager.Load<MusicAsset>({ "TestMusic.ogg", "TestMusic_0.ogg" });
	}

	void BootLevel::OnLoadNextLevel() {
		inputTokens.clear();
		engine->LoadLevel(LevelID::TestLevel);
	}

	void BootLevel::OnQuit() {
		inputTokens.clear();
		engine->Quit();
	}
}
