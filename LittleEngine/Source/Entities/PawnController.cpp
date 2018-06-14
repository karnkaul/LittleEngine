#include "stdafx.h"
#include "PawnController.h"
#include <vector>
#include "Engine/Logger/Logger.h"
#include "Pawn.h"
#include "SFMLInterface/InputHandler.h"
#include "SFMLInterface/Rendering/Renderer.h"

namespace Game {
	void PawnController::Tick(Fixed deltaTime, const InputHandler & inputHandler) {
		std::vector<KeyState> keys = inputHandler.GetPressed();
		for (const auto& key : keys) {
			OnKeyPressed(key, deltaTime);
		}
	}

	PawnController::PawnController(const std::string& name, Pawn& pawn)
		: Object(name), pawn(&pawn) {
	}

	PlayerController::PlayerController(Pawn& pawn) : PawnController("PawnController", pawn) {}

	void PlayerController::OnKeyPressed(const KeyState & state, Fixed deltaTime) {
		bool control = state.modifier.control;
		switch (state.GetKeyCode()) {
		case KeyCode::Left:
			if (control) pawn->GetTransform()->Rotate(deltaTime / 3);
			else pawn->GetTransform()->localPosition.x -= deltaTime;
			break;

		case KeyCode::Right:
			if (control) pawn->GetTransform()->Rotate(-deltaTime / 3);
			else pawn->GetTransform()->localPosition.x += deltaTime;
			break;

		case KeyCode::Up:
			pawn->GetTransform()->localPosition.y += deltaTime;
			break;

		case KeyCode::Down:
			pawn->GetTransform()->localPosition.y -= deltaTime;
			break;

		case KeyCode::Space:
			Logger::Log(*this, "Not implemented");
		}
	}
}
