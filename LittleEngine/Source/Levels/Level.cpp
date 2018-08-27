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

namespace Game {
	Level::Level(const std::string& name, Game::Engine& engine) : engine(engine), Object(name) {
		Logger::Log(*this, "Level created [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
	}

	bool Level::IsActorDestroyed(Actor::Ptr actor) {
		return actor == nullptr || actor->_destroyed;
	}

	void Level::Cleanup() {
		Utils::CleanVector<Actor::Ptr>(actors, &Level::IsActorDestroyed);
	}

	Level::~Level() {
		actors.clear();
		Logger::Log(*this, "Level destroyed");
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

	InputHandler & Level::GetInputHandler() const {
		return engine.GetInputHandler();
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

	const World & Level::GetWorld() const {
		return engine.GetWorld();
	}

	AssetManager & Level::GetAssetManager() const {
		return engine.GetAssetManager();
	}

	CollisionManager & Level::GetCollisionManager() {
		return collisionManager;
	}

	Actor::wPtr Level::SpawnActor(const std::string& name) {
		auto actor = std::make_shared<Game::Actor>(*this, name);
		actors.push_back(actor);
		return actor;
	}
}
