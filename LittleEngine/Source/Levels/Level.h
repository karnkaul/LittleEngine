#pragma once
#include "Engine/Object.h"
#include "Utils/Fixed.h"
#include "Engine/GameClock.h"
#include "Entities/Actor.h"
#include "Engine/Physics/CollisionManager.h"

namespace LittleEngine {
	class Engine;
	class World;
	class Input;
	struct RenderParams;
	class AssetManager;
	class InputHandler;

	// \brief Wrapper for level index
	enum class LevelID {
		BootLevel = 0,
		TestLevel = 1,
	};
	
	// \brief Base class for owning and spawning all Actors.
	// Provides APIs for InputHandler, AssetManager, CollisionManager, 
	// World, and Game Time
	class Level : public Object {
	public:
		virtual ~Level();
		virtual void FixedTick();
		virtual void Tick(Fixed deltaTime);
		virtual void Render(RenderParams& params);
		
		InputHandler& GetInputHandler() const;
		const World& GetWorld() const;
		AssetManager& GetAssetManager() const;
		CollisionManager& GetCollisionManager();
		int64_t LevelTimeMicroSeconds() const;
		int LevelTimeMilliSeconds() const;
		int GameTimeMilliSeconds() const;

		// \brief Safe API to spawn new Actors. All spawned Actors 
		// will be destroyed along with the Level, hence returns weak_ptr.
		Actor::wPtr SpawnActor(const std::string& name);
		// \brief Safe API to destroy existing Actors
		void DestroyActor(Actor::Ptr actor);

	protected:
		std::vector<Actor::Ptr> actors;
		GameClock clock;
		CollisionManager collisionManager;
		Engine* engine;
		
		Level(const std::string& name, Engine& engine);
		static bool IsActorDestroyed(Actor::Ptr actor);
		void Cleanup();
	};
}
