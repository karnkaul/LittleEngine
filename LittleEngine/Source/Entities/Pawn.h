#pragma once
#include "Actor.h"

namespace Game {
	// Forward declarations
	class PawnController;
	class Level;

	// Base class representing controllable Actors
	class Pawn : public Actor {
	public:
		Pawn(Level& level, std::string name);
		virtual ~Pawn();	// Needed for unique_ptr forward declaration
		virtual void FixedTick() override;
		virtual void Tick(Fixed deltaTime) override;
		void AddPlayerController();
	protected:
		std::unique_ptr<PawnController> controller = nullptr;
		class RenderComponent* renderComponent = nullptr;
	};
}
