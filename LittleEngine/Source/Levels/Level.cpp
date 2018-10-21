#include "le_stdafx.h"
#include <memory>
#include "Level.h"
#include "Engine/Engine.h"
#include "Engine/Logger/Logger.h"
#include "Vector2.h"
#include "Utils.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Input/InputHandler.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Assets.h"
#include "Entities/Actor.h"
#include "Entities/Player.h"
#include "Components/ControllerComponent.h"
#include "Components/RenderComponent.h"

namespace LittleEngine {
	Level::Level(const std::string& name) : Object(name) {
		Logger::Log(*this, GetNameInBrackets() + " (Level) created. [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
	}

	Level::~Level() {
		actors.clear();
		Logger::Log(*this, GetNameInBrackets() + " (Level) destroyed");
	}

	void Level::SetEngine(Engine & engine) {
		this->engine = &engine;
	}

	void Level::LoadAssets() {}

	void Level::Activate() {
		clock.Restart();
		Logger::Log(*this, GetNameInBrackets() + " (Level) activated. [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
	}

	void Level::FixedTick() {
		Logger::Log(*this, "Executing Fixed Tick", Logger::Severity::HOT);
		collisionManager.FixedTick();
		size_t countThisTurn = actors.size();
		for (size_t i = 0; i < countThisTurn; ++i) {
			if (!actors[i]->_destroyed) {
				actors[i]->FixedTick();
			}
		}
	}

	void Level::Tick(Fixed deltaTime) {
		// TEST
		if (clock.GetElapsedMilliSeconds() > 1000 && clock.GetElapsedMilliSeconds() < 1015) {
			Logger::Log(*this, "Player exists: " + std::to_string(player.lock() != nullptr));
		}

		Logger::Log(*this, "Executing Tick [" + std::to_string(actors.size()) + " actors]", Logger::Severity::HOT);
		size_t countThisTurn = actors.size();
		for (size_t i = 0; i < countThisTurn; ++i) {
			if (!actors[i]->_destroyed) {
				actors[i]->Tick(deltaTime);
			}
		}
	}

	void Level::Render(RenderParams& params) {
		GameUtils::CleanVector<Actor::Ptr>(actors, [](Actor::Ptr actor) { return !actor || actor->_destroyed; });
		for (const auto& actor : actors) {
			actor->Render(params);
		}
		PostRender(params);
	}

	void Level::PostRender(const RenderParams& params) {}

	void Level::Clear() {
		actors.clear();
		tokenHandler.Clear();
		OnClearing();
		Logger::Log(*this, GetNameInBrackets() + " (Level) deactivated. [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
	}

	void Level::OnClearing() {}

	void Level::RegisterScopedInput(const GameInput& gameInput, OnInput::Callback callback, const OnKey& type, bool consume) {
		OnInput::Token token = GetInputHandler().Register(gameInput, callback, type, consume);
		tokenHandler.AddToken(std::move(token));
	}

	void Level::DestroyActor(Actor::Ptr actor) {
		if (actor) {
			actor->_destroyed = true;
		}
		else {
			Logger::Log(*this, "Call to DestroyActor(nullptr)", Logger::Severity::Warning);
		}
	}

	Player::wPtr Level::GetOrSpawnPlayer(const std::string & texturePath, const AABBData & colliderBounds, const Vector2 & position, const Fixed & rotation) {
		Player::Ptr _player;
		if (!(_player = player.lock())) {
			_player = std::make_shared<Player>(*this, texturePath, colliderBounds, position, rotation);
			player = _player;
			actors.push_back(_player);
		}
		return player;
	}

	Player::wPtr Level::GetPlayer() {
		return player;
	}

	InputHandler & Level::GetInputHandler() const {
		return engine->GetInputHandler();
	}

	const World & Level::GetWorld() const {
		return engine->GetWorld();
	}

	AssetManager & Level::GetAssetManager() const {
		return engine->GetAssetManager();
	}

	AudioManager & Level::GetAudioManager() {
		return engine->GetAudioManager();
	}

	CollisionManager & Level::GetCollisionManager() {
		return collisionManager;
	}

	int64_t Level::LevelTimeMicroSeconds() const {
		return clock.GetElapsedMicroSeconds();
	}

	int Level::LevelTimeMilliSeconds() const {
		return clock.GetElapsedMilliSeconds();
	}

	int Level::GameTimeMilliSeconds() const {
		return clock.GetGameTimeMilliSeconds();
	}

	void Level::LoadLevel(LevelID levelID) {
		engine->LoadLevel(levelID);
	}

	void Level::Quit() {
		engine->Quit();
	}

	LevelID Level::GetActiveLevelID() const {
		return engine->GetActiveLevelID();
	}
}
