#pragma once
#include "Engine/Object.h"
#include "Utils/Fixed.h"
#include "Engine/GameClock.h"
#include "Entities/Actor.h"
#include "Engine/Physics/CollisionManager.h"

namespace Game {
	class Engine;
	class World;
	class Input;
	struct RenderParams;
	class AssetManager;
	class InputHandler;
	
	class Level : public Object {
	public:
		virtual ~Level();
		virtual void FixedTick();
		virtual void Tick(Fixed deltaTime);
		virtual void Render(RenderParams& params);
		
		InputHandler& GetInputHandler() const;
		int64_t LevelTimeMicroSeconds() const;
		int LevelTimeMilliSeconds() const;
		int GameTimeMilliSeconds() const;
		const World& GetWorld() const;
		AssetManager& GetAssetManager() const;
		CollisionManager& GetCollisionManager();

		Actor::wPtr SpawnActor(const std::string& name);
		void DestroyActor(Actor::Ptr actor);

	protected:
		std::vector<Actor::Ptr> actors;
		GameClock clock;
		CollisionManager collisionManager;
		Engine& engine;
		
		Level(const std::string& name, Engine& engine);
		static bool IsActorDestroyed(Actor::Ptr actor);
		void Cleanup();
	};
}
