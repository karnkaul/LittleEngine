#pragma once
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

		// \brief Safe API to spawn new Actors. All spawned Actors 
		// will be destroyed along with the Level, and Actors may be 
		// destroyed by other means. The weak pointer will indicate 
		// whether its associated Actor is still alive.
		template<typename T>
		std::shared_ptr<T> SpawnActor(const std::string& name, bool bSetEnabled, const Vector2& position = Vector2::Zero, const Fixed& rotation = Fixed::Zero) {
			static_assert(std::is_base_of<Actor, T>::value, "T must derive from Actor! Check output window for erroneous call");
			DEBUG_ASSERT(state != State::ACTIVE, "SpawnActor<T>() called in live game time! Use CloneActor<T>(prototype) instead");
			auto actor = std::make_shared<T>(*this, name, position, rotation);
			actor->ToggleActive(bSetEnabled);
			actors.push_back(actor);
			return actor;
		}

		// \brief Fast API to create copies of Actors. Create prototypes once 
		// and then duplicate using Clone. 
		template<typename T>
		std::shared_ptr<T> CloneActor(const T& prototype) {
			static_assert(std::is_base_of<Actor, T>::value, "T must derive from Actor! Check output window for erroneous call");
			auto actor = std::make_shared<T>(*this, prototype);
			actors.push_back(actor);
			return actor;
		}

		// \brief Safe API to destroy existing Actors
		void DestroyActor(Actor::Ptr actor);
		// \brief Safe API to spawn Player.
		Player::Ptr GetOrSpawnPlayer(const std::string& texturePath, const AABBData& colliderBounds, const Vector2& position = Vector2::Zero, const Fixed& rotation = Fixed::Zero);
		Player::wPtr GetPlayer();

	protected:
		enum class State {
			INVALID		= 0,
			IDLE		= 1,
			LOADING		= 2,
			ACTIVE		= 3,
			ERROR		= 10
		};

		std::vector<Actor::Ptr> actors;
		GameClock clock;
		CollisionManager collisionManager;
		Player::wPtr player;
		Engine* engine;
		
		Level(const std::string& name);

	public:
		
	protected:
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
		friend class Engine;
		friend class LevelManager;

		void Render(RenderParams& params);
		void SetEngine(Engine& engine);
		void Activate();
		void Clear();

		State state = State::INVALID;
		GameUtils::TokenHandler<OnInput::Token> tokenHandler;
	};
}
