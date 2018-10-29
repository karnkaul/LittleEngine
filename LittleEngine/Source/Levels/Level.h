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
		// will be destroyed along with the Level, hence returns weak_ptr.
		template<typename T>
		std::weak_ptr<T> SpawnActor(const std::string& name, const Vector2& position = Vector2::Zero, const Fixed& rotation = Fixed::Zero) {
			static_assert(std::is_base_of<Actor, T>::value, "T must derive from Actor! Check output window for erroneous call");
			auto actor = std::make_shared<T>(*this, name, position, rotation);
			actors.push_back(actor);
			return actor;
		}
		// \brief Safe API to destroy existing Actors
		void DestroyActor(Actor::Ptr actor);
		// \brief Safe API to spawn Player.
		Player::wPtr GetOrSpawnPlayer(const std::string& texturePath, const AABBData& colliderBounds, const Vector2& position = Vector2::Zero, const Fixed& rotation = Fixed::Zero);
		Player::wPtr GetPlayer();

	protected:
		std::vector<Actor::Ptr> actors;
		GameClock clock;
		CollisionManager collisionManager;
		Player::wPtr player;
		Engine* engine;
		
		Level(const std::string& name);

		void SetEngine(Engine& engine);
		// \brief Override to initiate level (spawn destroyed/first-time actors, etc)
		virtual void Activate();
		// \brief Override for callback every fixed time slice
		virtual void FixedTick();
		// \brief Override for callback every frame
		virtual void Tick(Fixed deltaTime);
		void Render(RenderParams& params);
		// \brief Override to obtain RenderParams for current frame (no need to call base class implementation)
		virtual void PostRender(const RenderParams& params);
		void Clear();
		// \brief Override for callback before level is deactivated (no need to call base class implementation)
		virtual void OnClearing();
		
		// \brief Registers corresponding input scoped to Level's activity; Token is destroyed on Clear()
		void RegisterScopedInput(const GameInput& gameInput, OnInput::Callback callback, const OnKey& type, bool consume = false);

		friend class Engine;
		friend class LevelManager;

	private:
		GameUtils::TokenHandler<OnInput::Token> tokenHandler;
	};
}
