#include "stdafx.h"
#include <memory>
#include "Level.h"
#include "Engine/Engine.h"
#include "Engine/Logger/Logger.h"
#include "Utils/Vector2.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Input/InputHandler.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Assets.h"
#include "Entities/Actor.h"

namespace Game {
	Level::Level(const std::string& name, Game::Engine& engine)
		: engine(&engine), Object(name) {
		Logger::Log(*this, "Level created [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
	}

	bool Level::IsActorDestroyed(const std::shared_ptr<Actor>& actor) {
		return actor->_destroyed;
	}

	void Level::Cleanup() {
		// Move all destroyed actors to end of vector
		auto iter = std::remove_if(actors.begin(), actors.end(), &Level::IsActorDestroyed);
		// Erase all destroyed actors
		actors.erase(iter, actors.end());
	}

	Level::~Level() {
		actors.clear();
		Logger::Log(*this, "Level destroyed");
	}

	void Level::FixedTick() {
		Logger::Log(*this, "Executing Fixed Tick", Logger::Severity::HOT);
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

	void Level::DestroyActor(const std::shared_ptr<Actor>& actor) {
		actor->_destroyed = true;
	}

	const Input& Level::GetInput() const {
		return engine->GetInput();
	}

	InputHandler & Level::GetInputHandler() const {
		return engine->GetInputHandler();
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
		return engine->GetWorld();
	}

	AssetManager & Level::GetAssetManager() const {
		return engine->GetAssetManager();
	}

	std::shared_ptr<Game::Actor> Level::NewActor(const std::string& name) {
		auto actor = std::make_shared<Game::Actor>(*this, name);
		actors.push_back(actor);
		return actor;
	}
}
