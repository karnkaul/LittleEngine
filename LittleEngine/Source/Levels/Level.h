#pragma once
#include <unordered_map>
#include "Fixed.h"
#include "TokenHandler.hpp"
#include "Engine/Object.h"
#include "Engine/GameClock.h"
#include "Entities/Actor.h"
#include "Entities/Player.h"
#include "Components/Physics/CollisionManager.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Input/InputHandler.h"
#include "Engine/Logger/Logger.h"
#include "Spawner.h"

namespace LittleEngine {
	class Engine;
	class Input;
	struct RenderParams;
	class AssetManager;

	// \brief Wrapper for level index
	using LevelID = int;
	#define BOOTLEVEL 0
	
	// \brief Base class for owning and spawning all Actors.
	// Provides APIs for InputHandler, AssetManager, CollisionManager, 
	// World, and Game Time. Constructor should load required assets.
	class Level : public Object {
	private:
		enum class State {
			INVALID = 0,
			IDLE = 1,
			LOADING = 2,
			ACTIVE = 3,
			ERROR = 10
		};

	public:
		static const int MAX_LEVEL_IDX = 1;
	private:
		static int nextActorID;

	protected:
		GameClock m_clock;
		Engine* m_pEngine;
		std::unique_ptr<class UIController> m_uUIController;
		GameUtils::TokenHandler<OnInput::Token> m_tokenHandler;

	private:
		CollisionManager m_collisionManager;
		std::unordered_map<int, Actor::Ptr> m_actorMap;
		std::unordered_map<int, Actor::Ptr> m_prototypeMap;
		State m_state = State::INVALID;
	
	public:
		virtual ~Level();
		
		// Override to load assets and spawn prototypes before level activates
		virtual void LoadAndSpawn();
		
		UIController& GetUIController() const;
		InputHandler& GetInputHandler() const;
		AssetManager& GetAssetManager() const;
		AudioManager& GetAudioManager();
		CollisionManager& GetCollisionManager();
		int64_t LevelTimeMicroSeconds() const;
		int LevelTimeMilliSeconds() const;
		int GameTimeMilliSeconds() const;

		// \brief Instructs Engine to load LevelID next frame
		void LoadLevel(LevelID levelID);
		// \brief Instructs Engine to Quit next frame
		void Quit();
		// \brief Returns active level's ID
		LevelID GetActiveLevelID() const;

		// \brief Safe API to spawn new Actors. All spawned Actors will be destroyed along with the Level
		// Actors may also be prematurely destroyed. Ensure to check Actor* validity via IsAlive(actorID)
		template<typename T>
		T* SpawnActor(const std::string& name, bool bSetEnabled, const Vector2& position = Vector2::Zero, const Fixed& orientation = Fixed::Zero) {
			static_assert(std::is_base_of<Actor, T>::value, "T must derive from Actor! Check output window for erroneous call");
			if (m_state == State::ACTIVE) Logger::Log(*this, "SpawnActor called in live time; SpawnPrototypes on init and use CloneActor here instead", Logger::Severity::Warning);
			auto actor = NewActor<T>(name, bSetEnabled, position, orientation);
			T* ret = actor.get();
			m_actorMap.emplace(actor->m_actorID, std::move(actor));
			return ret;
		}

		// \brief Prototypes are not Ticked or Rendered, and are not destroyed till the owning Level is
		template<typename T>
		T* SpawnPrototype(const std::string& name, const Vector2& position = Vector2::Zero, const Fixed& orientation = Fixed::Zero) {
			static_assert(std::is_base_of<Actor, T>::value, "T must derive from Actor! Check output window for erroneous call");
			auto actor = NewActor<T>(name, false, position, orientation);
			T* ret = actor.get();
			m_prototypeMap.emplace(actor->m_actorID, std::move(actor));
			return ret;
		}

		// \brief Fast API to create copies of Actors. Create prototypes once 
		// and then duplicate using Clone. 
		template<typename T>
		T* CloneActor(const T& prototype) {
			static_assert(std::is_base_of<Actor, T>::value, "T must derive from Actor! Check output window for erroneous call");
			auto actor = std::make_unique<T>();
			actor->InitActor(*this, nextActorID++, prototype);
			T* ret = actor.get();
			m_actorMap.emplace(actor->m_actorID, std::move(actor));
			return ret;
		}

		// Returns nullptr if no Actor with corresponding actorID exists
		Actor* FindActor(int actorID) const;
		// Returns false if no Actor with corresponding actorID exists
		bool IsAlive(int actorID) const;
		// \brief Safe API to destroy existing Actors
		bool DestroyActor(int actorID);

	protected:
		Level(const std::string& name);

		// \brief Override to initiate level (spawn destroyed/first-time actors, etc)
		virtual void OnActivated();
		// \brief Override for callback every fixed time slice
		virtual void FixedTick();
		// \brief Override for callback every frame
		virtual void Tick(Fixed deltaMS);
		// \brief Override to obtain RenderParams for current frame (no need to call base class implementation)
		virtual void PostRender();
		// \brief Override for callback before level is deactivated (no need to call base class implementation)
		virtual void OnClearing();
		
		// \brief Registers corresponding input scoped to Level's activity; Token is destroyed on Clear()
		void RegisterScopedInput(const GameInput& gameInput, OnInput::Callback callback, const OnKey& type, bool consume = false);

	private:
		template<typename T>
		std::unique_ptr<T> NewActor(const std::string& name, bool bSetEnabled, const Vector2& position = Vector2::Zero, const Fixed& orientation = Fixed::Zero) {
			auto actor = std::make_unique<T>();
			actor->InitActor(*this, nextActorID++, name, position, orientation);
			actor->ToggleActive(bSetEnabled);
			return std::move(actor);
		}

		void Render();
		void SetEngine(Engine& engine);
		void Activate();
		void Clear();

		friend class Engine;
		friend class LevelManager;
	};
}
