#include "le_stdafx.h"
#include "ControllerComponent.h"
#include <functional>
#include "Component.h"
#include "Entities/Actor.h"
#include "Engine/Logger/Logger.h"
#include "Engine/Input/InputHandler.h"
#include "SFMLInterface/Rendering/Renderable.h"
#include "Components/RenderComponent.h"
#include "Transform.h"
#include "SFMLInterface/Input.h"
#include "SFMLInterface/Graphics.h"
#include "Levels/Level.h"

namespace LittleEngine {
	namespace _ControllerComponent {
		LittleEngine::Logger::Severity s = LittleEngine::Logger::Severity::Debug;
		void Test() {
			LittleEngine::Logger::Log("Another left detected!", s);
		}

		void Test2() {
			LittleEngine::Logger::Log("Consuming left detected! (no other Lefts should be triggered)", s);
		}
	}

	namespace {
		bool _bDeletedToken = false;

		void ClampPosition(Vector2& position, const GameUtils::Rect2& worldBounds, const GameUtils::Vector2& padding) {
			if ((position.x - padding.x) < worldBounds.lower.x) position.x = worldBounds.lower.x + padding.x;
			if ((position.x + padding.x) > worldBounds.upper.x) position.x = worldBounds.upper.x - padding.x;
			if ((position.y - padding.y) < worldBounds.lower.y) position.y = worldBounds.lower.y + padding.y;
			if ((position.y + padding.y) > worldBounds.upper.y) position.y = worldBounds.upper.y - padding.y;
		}
	}

	ControllerComponent::ControllerComponent(Actor & actor) : Component(actor, "ControllerComponent") {
		Init();
	}

	ControllerComponent::ControllerComponent(Actor& owner, const ControllerComponent & prototype) : Component(owner, prototype), m_pRenderer(prototype.m_pRenderer), m_pInputHandler(prototype.m_pInputHandler) {
		Init();
	}

	ControllerComponent::~ControllerComponent() {
		m_tokens.clear();
	}

	void ControllerComponent::Tick(const Fixed& deltaMS) {
		prevDeltaMS = deltaMS;
		Actor& actor = GetActor();

		Rect2 worldBounds = Graphics::GetWorldRect();
		Vector2 padding = GetRenderPadding();
		ClampPosition(actor.GetTransform().localPosition, worldBounds, padding);

		// TESTS
		if (actor.GetActiveLevel().LevelTimeMilliSeconds() > 2000 && !_bDeletedToken) {
			_bDeletedToken = true;
			if (!m_tokens.empty()) m_tokens.pop_back();
		}
	}

	Component::Ptr ControllerComponent::UClone(Actor& owner) const {
		return std::make_unique<ControllerComponent>(owner, *this);
	}

	void ControllerComponent::Init() {
		m_tokens.clear();
		this->m_pInputHandler = &GetActor().GetActiveLevel().GetInputHandler();
		m_tokens.push_back(m_pInputHandler->Register(GameInput::Left, std::bind(&ControllerComponent::OnLeft, this), OnKey::Held));
		m_tokens.push_back(m_pInputHandler->Register(GameInput::Right, std::bind(&ControllerComponent::OnRight, this), OnKey::Held));
		m_tokens.push_back(m_pInputHandler->Register(GameInput::Up, std::bind(&ControllerComponent::OnUp, this), OnKey::Held));
		m_tokens.push_back(m_pInputHandler->Register(GameInput::Down, std::bind(&ControllerComponent::OnDown, this), OnKey::Held));
		m_pRenderer = GetActor().GetComponent<RenderComponent>();

		// Tests
		_bDeletedToken = false;
		m_tokens.push_back(m_pInputHandler->Register(GameInput::Left, &_ControllerComponent::Test, OnKey::Released));
		m_tokens.push_back(m_pInputHandler->Register(GameInput::Left, &_ControllerComponent::Test2, OnKey::Released, true));
	}

	Vector2 ControllerComponent::GetRenderPadding() {
		if (!m_pRenderer) {
			m_pRenderer = GetActor().GetComponent<RenderComponent>();
		}
		if (!m_pRenderer) {
			Logger::Log(*this, "ControllerComponent's owning Actor does not have a RenderComponent", Logger::Severity::Warning);
			return Vector2::Zero;
		}
		return m_pRenderer->GetBounds().upper;
	}

	void ControllerComponent::OnLeft() {
		if (m_pInputHandler->IsKeyPressed(GameInput::LB)) {
			GetActor().GetTransform().Rotate(prevDeltaMS * Fixed::OneThird);
		}
		else {
			GetActor().GetTransform().localPosition.x -= prevDeltaMS;
		}
	}

	void ControllerComponent::OnRight() {
		if (m_pInputHandler->IsKeyPressed(GameInput::LB)) {
			GetActor().GetTransform().Rotate(-prevDeltaMS * Fixed::OneThird);
		}
		else {
			GetActor().GetTransform().localPosition.x += prevDeltaMS;
		}
	}

	void ControllerComponent::OnUp() {
		GetActor().GetTransform().localPosition.y += prevDeltaMS;
	}

	void ControllerComponent::OnDown() {
		GetActor().GetTransform().localPosition.y -= prevDeltaMS;
	}
}
