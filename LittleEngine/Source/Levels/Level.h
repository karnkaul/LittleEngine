#pragma once
#include "Engine/Object.h"
#include "Utils/Fixed.h"
#include "Engine/GameClock.h"

namespace Game {
	class Engine;
	class World;
	class Actor;
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

		std::shared_ptr<Actor> NewActor(const std::string& name);
		void DestroyActor(const std::shared_ptr<Actor>& actor);

	protected:
		std::vector<std::shared_ptr<Actor> > actors;
		GameClock clock;
		Engine* engine;
		
		Level(const std::string& name, Engine& engine);
		static bool IsActorDestroyed(const std::shared_ptr<Actor>& actor);
		void Cleanup();
	};
}
