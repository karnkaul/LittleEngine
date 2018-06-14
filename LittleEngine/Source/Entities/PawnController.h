#pragma once
#include "Utils/Fixed.h"
#include "Engine/Object.h"

namespace Game {
	// Forward Declarations
	class Pawn;
	class InputHandler;
	struct KeyState;

	// Base class for controlling Pawns
	class PawnController : public Object {
	public:
		void Tick(Fixed deltaTime, const InputHandler& inputHandler);
	protected:
		Pawn * pawn;
		PawnController(const std::string& name, Pawn& pawn);
		virtual void OnKeyPressed(const KeyState& state, Fixed deltaTime) = 0;
	private:
		PawnController(const PawnController&) = delete;
		PawnController& operator=(const PawnController&) = delete;
	};

	// Default player controller
	class PlayerController : public PawnController {
	public:
		PlayerController(Pawn& pawn);
		virtual void OnKeyPressed(const KeyState& state, Fixed deltaTime) override;
	};
}
