#include "stdafx.h"
#include <memory>
#include "Level.h"
#include "Engine/Engine.h"
#include "Engine/Logger/Logger.h"
#include "Utils/Vector2.h"
#include "Utils/Utils.h"
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
	Level::Level(const std::string& name, LittleEngine::Engine& engine) : Object(name) {
		this->engine = &engine;
		Logger::Log(*this, GetNameInBrackets() + " (Level) created [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
	}

	bool Level::IsActorDestroyed(Actor::Ptr actor) {
		return actor == nullptr || actor->_destroyed;
	}

	void Level::Cleanup() {
		Utils::CleanVector<Actor::Ptr>(actors, &Level::IsActorDestroyed);
	}

	Level::~Level() {
		actors.clear();
		Logger::Log(*this, GetNameInBrackets() + " (Level) destroyed");
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
		Cleanup();
		for (const auto& actor : actors) {
			actor->Render(params);
		}
	}

	void Level::DestroyActor(Actor::Ptr actor) {
		if (actor != nullptr) {
			actor->_destroyed = true;
		}
		else {
			Logger::Log(*this, "Call to DestroyActor(nullptr)", Logger::Severity::Warning);
		}
	}

	Actor::wPtr Level::SpawnActor(const std::string& name, const Vector2& position, const Fixed& rotation) {
		auto actor = std::make_shared<Actor>(*this, name, position, rotation);
		actors.push_back(actor);
		return actor;
	}

	Player::wPtr Level::GetOrSpawnPlayer(const std::string & texturePath, const AABBData & colliderBounds, const Vector2 & position, const Fixed & rotation) {
		Player::Ptr _player = nullptr;
		if ((_player = player.lock()) == nullptr) {
			_player = std::make_shared<Player>(*this, texturePath, colliderBounds, position, rotation);
			player = _player;
			auto _p = std::dynamic_pointer_cast<Actor>(_player);
			actors.push_back(_p);
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
}
