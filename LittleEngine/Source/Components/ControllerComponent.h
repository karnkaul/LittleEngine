#pragma once
#include <vector>
#include "Component.h"
#include "Delegate.hpp"
#include "Vector2.h"
#include "Engine/Input/InputHandler.h"

namespace LittleEngine {
	class Actor;
	struct KeyState;

	using Vector2 = GameUtils::Vector2;
	
	// \brief Player Controller Component
	class ControllerComponent : public Component {
	private:
		Fixed prevDeltaMS = 0;
		std::vector<OnInput::Token> m_tokens;
		class RenderComponent* m_pRenderer = nullptr;
		InputHandler* m_pInputHandler;

	public:
		ControllerComponent(Actor& actor);
		ControllerComponent(Actor& owner, const ControllerComponent& prototype);
		virtual ~ControllerComponent();
		
		virtual void Tick(const Fixed& deltaMS) override;
		virtual Component::Ptr UClone(Actor& owner) const override;

	private:
		void Init();
		Vector2 GetRenderPadding();
		void OnLeft();
		void OnRight();
		void OnUp();
		void OnDown();
	};
}
