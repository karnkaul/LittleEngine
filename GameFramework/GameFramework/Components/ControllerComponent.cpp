#include "stdafx.h"
#include "ControllerComponent.h"
#include "RenderComponent.h"
#include "LittleEngine/GFX/GFX.h"
#include "LittleEngine/Game/Entity.h"

namespace LittleEngine
{
namespace
{
void ClampPosition(Vector2& outPosition, const Vector2& padding)
{
	Vector2 worldSize = GFX::GetViewSize() * Fixed::OneHalf;
	if (outPosition.x + padding.x > worldSize.x)
		outPosition.x = (worldSize.x - padding.x);
	else if (outPosition.x - padding.x < -worldSize.x)
		outPosition.x = -(worldSize.x - padding.x);
	if (outPosition.y + padding.y > worldSize.y)
		outPosition.y = (worldSize.y - padding.y);
	else if (outPosition.y - padding.y < -worldSize.y)
		outPosition.y = -(worldSize.y - padding.y);
}
} // namespace

void ControllerComponent::Reset()
{
	m_displacement = Vector2::Zero;
	m_rotation = Fixed::Zero;
}

TimingType ControllerComponent::GetComponentTiming() const
{
	return TimingType::PRE;
}

void ControllerComponent::OnCreated()
{
	m_name = "Controller";
	m_pRenderComponent = m_pOwner->GetComponent<RenderComponent>();
	if (!m_pRenderComponent)
		LogE(LogName() + m_pOwner->LogName() + " has ControllerComponent but no RenderComponent!");
	BindInput(std::bind(&ControllerComponent::OnInput, this, _1));
	Reset();
}

void ControllerComponent::Tick(Time dt)
{
	if (Maths::Abs(m_rotation) > Fixed::Zero)
	{
		m_pOwner->m_transform.Rotate(m_rotation * m_angularSpeed * dt.AsMilliseconds());
	}
	if (m_displacement.SqrMagnitude() > 0.0f)
	{
		m_pOwner->m_transform.localPosition += (m_displacement * m_linearSpeed * dt.AsMilliseconds());
	}
	if (m_pRenderComponent)
		ClampPosition(m_pOwner->m_transform.localPosition,
					  m_pRenderComponent->m_pSFPrimitive->GetBounds().GetSize() * Fixed::OneHalf);
}

void ControllerComponent::SetEnabled(bool bEnabled)
{
	Component::SetEnabled(bEnabled);

	Reset();
}

bool ControllerComponent::OnInput(const EngineInput::Frame& frame)
{
	if (!m_bEnabled)
		return false;

	bool bModifier = frame.IsHeld(GameInputType::LB);
	m_rotation = Fixed::Zero;
	m_displacement = Vector2::Zero;

	// Horizontal
	{
		if (frame.IsHeld(GameInputType::Left))
		{
			if (bModifier)
				m_rotation = 1;
			else
				m_displacement.x = -1;
		}

		else if (frame.IsHeld(GameInputType::Right))
		{
			if (bModifier)
				m_rotation = -1;
			else
				m_displacement.x = 1;
		}
	}

	// Vertical
	{
		if (frame.IsHeld(GameInputType::Up))
		{
			m_displacement.y += 1;
		}

		else if (frame.IsHeld(GameInputType::Down))
		{
			m_displacement.y -= 1;
		}
	}

	if (m_displacement.SqrMagnitude() > 0.0f)
		m_displacement.Normalise();
	return false;
}
} // namespace LittleEngine
