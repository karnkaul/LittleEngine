#pragma once
#include <unordered_map>
#include "Fixed.h"
#include "TokenHandler.hpp"
#include "Engine/Object.h"
#include "Engine/GameClock.h"
#include "Entities/Actor.h"
#include "Entities/Player.h"
#include "Engine/Physics/CollisionManager.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Input/InputHandler.h"
#include "Engine/Logger/Logger.h"
#include "Spawner.h"

namespace LittleEngine {
	class Engine;
	class World;
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
	public:
		const static int MAX_LEVEL_IDX = 1;

		virtual ~Level();
		
		// Override to load assets before level activates
		virtual void LoadAssets();

		InputHandler& GetInputHandler() const;
		const World& GetWorld() const;
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
		T* SpawnActor(const std::string& name, bool bSetEnabled, const Vector2& position = Vector2::Zero, const Fixed& rotation = Fixed::Zero) {
			static_assert(std::is_base_of<Actor, T>::value, "T must derive from Actor! Check output window for erroneous call");
			DEBUG_ASSERT(state != State::ACTIVE, "SpawnActor<T>() called in live game time! Use CloneActor<T>(prototype) instead");
			auto actor = std::make_unique<T>(*this, name, position, rotation);
			actor->actorID = nextActorID++;
			actor->ToggleActive(bSetEnabled);
			T* ret = actor.get();
			actorMap.emplace(actor->actorID, std::move(actor));
			return ret;
		}

		// \brief Fast API to create copies of Actors. Create prototypes once 
		// and then duplicate using Clone. 
		template<typename T>
		T* CloneActor(const T& prototype) {
			static_assert(std::is_base_of<Actor, T>::value, "T must derive from Actor! Check output window for erroneous call");
			auto actor = std::make_unique<T>(*this, prototype);
			actor->actorID = nextActorID++;
			T* ret = actor.get();
			actorMap.emplace(actor->actorID, std::move(actor));
			return ret;
		}

		// Returns nullptr if no Actor with corresponding actorID exists
		Actor* FindActor(int actorID) const;
		// Returns false if no Actor with corresponding actorID exists
		bool IsAlive(int actorID) const;
		// \brief Safe API to destroy existing Actors
		bool DestroyActor(int actorID);

	protected:
		GameClock clock;
		Engine* engine;

		Level(const std::string& name);

		// \brief Override to initiate level (spawn destroyed/first-time actors, etc)
		virtual void OnActivated();
		// \brief Override for callback every fixed time slice
		virtual void FixedTick();
		// \brief Override for callback every frame
		virtual void Tick(Fixed deltaTime);
		// \brief Override to obtain RenderParams for current frame (no need to call base class implementation)
		virtual void PostRender(const RenderParams& params);
		// \brief Override for callback before level is deactivated (no need to call base class implementation)
		virtual void OnClearing();
		
		// \brief Registers corresponding input scoped to Level's activity; Token is destroyed on Clear()
		void RegisterScopedInput(const GameInput& gameInput, OnInput::Callback callback, const OnKey& type, bool consume = false);

	private:
		enum class State {
			INVALID = 0,
			IDLE = 1,
			LOADING = 2,
			ACTIVE = 3,
			ERROR = 10
		};

		friend class Engine;
		friend class LevelManager;

		static int nextActorID;

		CollisionManager collisionManager;
		GameUtils::TokenHandler<OnInput::Token> tokenHandler;

	private:
		std::unordered_map<int, Actor::Ptr> actorMap;
		State state = State::INVALID;

		void Render(RenderParams& params);
		void SetEngine(Engine& engine);
		void Activate();
		void Clear();
	};
}
