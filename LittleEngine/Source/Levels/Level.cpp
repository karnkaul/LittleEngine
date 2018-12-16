#include "le_stdafx.h"
#include <memory>
#include "Level.h"
#include "Engine/Engine.h"
#include "Engine/Logger/Logger.h"
#include "Vector2.h"
#include "Utils.h"
#include "Engine/Engine.h"
#include "Engine/Input/InputHandler.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Assets.h"
#include "Entities/Actor.h"
#include "Entities/Player.h"
#include "UI/UIController.h"
#include "Components/ControllerComponent.h"
#include "Components/RenderComponent.h"
#include "Misc/Stopwatch.h"
#include "Spawner.h"

namespace LittleEngine {
	int Level::nextActorID = 1000;
	constexpr int RESERVE_HASH_BUCKETS = 2048;

	Level::Level(const std::string& name) : Object(name) {
		m_actorMap.reserve(RESERVE_HASH_BUCKETS);
		Logger::Log(*this, GetNameInBrackets() + " (Level) created. [GameTime: " + m_clock.ToString(m_clock.GetGameTimeMilliSeconds()) + "]");
	}

	Level::~Level() {
		m_actorMap.clear();
		m_prototypeMap.clear();
		Logger::Log(*this, GetNameInBrackets() + " (Level) destroyed");
	}

	void Level::SetEngine(Engine & engine) {
		this->m_pEngine = &engine;
		m_state = State::IDLE;
	}

	void Level::LoadAssets() {}

	void Level::SpawnPrototypes() {}

	UIController& Level::GetUIController() const {
		return *m_uUIController;
	}

	void Level::Activate() {
		m_uUIController = std::make_unique<UIController>(*this);
		m_state = State::ACTIVE;
		OnActivated();
		m_clock.Restart();
		Logger::Log(*this, GetNameInBrackets() + " (Level) activated. [GameTime: " + m_clock.ToString(m_clock.GetGameTimeMilliSeconds()) + "]");
	}

	void Level::OnActivated() {}

	void Level::FixedTick() {
		Logger::Log(*this, "Executing Fixed Tick", Logger::Severity::HOT);
		m_collisionManager.FixedTick();

		for (auto & iter : m_actorMap) {
			Actor::Ptr& actor = iter.second;
			if (!actor->m_bDestroyed && actor->m_bEnabled) {
				actor->FixedTick();
			}
		}
	}

	void Level::Tick(Fixed deltaMS) {
		Logger::Log(*this, "Executing Tick [" + Strings::ToString(m_actorMap.size()) + " actors]", Logger::Severity::HOT);

		for (auto & iter : m_actorMap) {
			Actor::Ptr& actor = iter.second;
			if (!actor->m_bDestroyed && actor->m_bEnabled) {
				actor->Tick(deltaMS);
			}
		}
		if (m_uUIController) m_uUIController->Tick(deltaMS);
	}

	void Level::Render() {
		STOPWATCH_START("Render");
		GameUtils::CleanMap<int, Actor::Ptr>(m_actorMap, [](Actor::Ptr& actor) { return !actor || actor->m_bDestroyed; });
		for (auto & iter : m_actorMap) {
			Actor::Ptr& actor = iter.second;
			if (actor->m_bEnabled) {
				actor->Render();
			}
		}
		if (m_uUIController) m_uUIController->Render();
		STOPWATCH_STOP();
		STOPWATCH_START("Post Render");
		PostRender();
		STOPWATCH_STOP();
	}

	void Level::PostRender() {}

	void Level::Clear() {
		OnClearing();
		m_uUIController = nullptr;
		m_actorMap.clear();
		Spawner::Cleanup();
		m_tokenHandler.Clear();
		Logger::Log(*this, GetNameInBrackets() + " (Level) deactivated. [GameTime: " + m_clock.ToString(m_clock.GetGameTimeMilliSeconds()) + "]");
		m_state = State::IDLE;
	}

	void Level::OnClearing() {
	}

	void Level::RegisterScopedInput(const GameInput& gameInput, OnInput::Callback callback, const OnKey& type, bool consume) {
		OnInput::Token token = GetInputHandler().Register(gameInput, callback, type, consume);
		m_tokenHandler.AddToken(std::move(token));
	}

	Actor* Level::FindActor(int actorID) const {
		auto search = m_actorMap.find(actorID);
		if (search != m_actorMap.end()) {
			return search->second.get();
		}
		return nullptr;
	}

	bool Level::IsAlive(int actorID) const {
		return m_actorMap.find(actorID) != m_actorMap.end();
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
		return m_pEngine->GetInputHandler();
	}

	AssetManager & Level::GetAssetManager() const {
		return m_pEngine->GetAssetManager();
	}

	AudioManager & Level::GetAudioManager() {
		return m_pEngine->GetAudioManager();
	}

	CollisionManager & Level::GetCollisionManager() {
		return m_collisionManager;
	}

	int64_t Level::LevelTimeMicroSeconds() const {
		return m_clock.GetElapsedMicroSeconds();
	}

	int Level::LevelTimeMilliSeconds() const {
		return m_clock.GetElapsedMilliSeconds();
	}

	int Level::GameTimeMilliSeconds() const {
		return m_clock.GetGameTimeMilliSeconds();
	}

	void Level::LoadLevel(LevelID levelID) {
		m_pEngine->LoadLevel(levelID);
	}

	void Level::Quit() {
		m_pEngine->Quit();
	}

	LevelID Level::GetActiveLevelID() const {
		return m_pEngine->GetActiveLevelID();
	}
}
