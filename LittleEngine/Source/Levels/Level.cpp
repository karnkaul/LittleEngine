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
#include "Misc/Stopwatch.h"
#include "Spawner.h"

namespace LittleEngine {
	int Level::nextActorID = 1000;
	constexpr int RESERVE_HASH_BUCKETS = 2048;

	Level::Level(const std::string& name) : Object(name) {
		actorMap.reserve(RESERVE_HASH_BUCKETS);
		Logger::Log(*this, GetNameInBrackets() + " (Level) created. [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
	}

	Level::~Level() {
		actorMap.clear();
		Logger::Log(*this, GetNameInBrackets() + " (Level) destroyed");
	}

	void Level::SetEngine(Engine & engine) {
		this->engine = &engine;
		state = State::IDLE;
	}

	void Level::LoadAssets() {}

	void Level::Activate() {
		OnActivated();
		state = State::ACTIVE;
		clock.Restart();
		Logger::Log(*this, GetNameInBrackets() + " (Level) activated. [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
	}

	void Level::OnActivated() {}

	void Level::FixedTick() {
		Logger::Log(*this, "Executing Fixed Tick", Logger::Severity::HOT);
		collisionManager.FixedTick();

		for (auto & iter : actorMap) {
			Actor::Ptr& actor = iter.second;
			if (!actor->_bDestroyed && actor->_bEnabled) {
				actor->FixedTick();
			}
		}
	}

	void Level::Tick(Fixed deltaTime) {
		Logger::Log(*this, "Executing Tick [" + Strings::ToString(actorMap.size()) + " actors]", Logger::Severity::HOT);

		for (auto & iter : actorMap) {
			Actor::Ptr& actor = iter.second;
			if (!actor->_bDestroyed && actor->_bEnabled) {
				actor->Tick(deltaTime);
			}
		}
	}

	void Level::Render(RenderParams& params) {
		STOPWATCH_START("Render");
		GameUtils::CleanMap<int, Actor::Ptr>(actorMap, [](Actor::Ptr& actor) { return !actor || actor->_bDestroyed; });
		for (auto & iter : actorMap) {
			Actor::Ptr& actor = iter.second;
			if (actor->_bEnabled) {
				actor->Render(params);
			}
		}
		STOPWATCH_STOP();
		STOPWATCH_START("Post Render");
		PostRender(params);
		STOPWATCH_STOP();
	}

	void Level::PostRender(const RenderParams& params) {}

	void Level::Clear() {
		OnClearing();
		actorMap.clear();
		Spawner::Cleanup();
		tokenHandler.Clear();
		Logger::Log(*this, GetNameInBrackets() + " (Level) deactivated. [GameTime: " + clock.ToString(clock.GetGameTimeMilliSeconds()) + "]");
		state = State::IDLE;
	}

	void Level::OnClearing() {
	}

	void Level::RegisterScopedInput(const GameInput& gameInput, OnInput::Callback callback, const OnKey& type, bool consume) {
		OnInput::Token token = GetInputHandler().Register(gameInput, callback, type, consume);
		tokenHandler.AddToken(std::move(token));
	}

	Actor* Level::FindActor(int actorID) const {
		auto search = actorMap.find(actorID);
		if (search != actorMap.end()) {
			return search->second.get();
		}
		return nullptr;
	}

	bool Level::IsAlive(int actorID) const {
		return actorMap.find(actorID) != actorMap.end();
	}

	bool Level::DestroyActor(int actorID) {
		Actor* search = FindActor(actorID);
		if (search) {
			search->Destruct();
			return true;
		}
		Logger::Log(*this, "DestroyActor(): ActorID [" + Strings::ToString(actorID) + "] does not exist", Logger::Severity::Warning);
		return false;
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
