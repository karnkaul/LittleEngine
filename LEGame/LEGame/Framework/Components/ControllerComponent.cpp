#include "stdafx.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "LEGame/Model/World/Entity.h"
#include "LEGame/Model/GameManager.h"
#include "ControllerComponent.h"
#include "RenderComponent.h"

namespace LittleEngine
{
namespace
{
void ClampPosition(Vector2& outPosition, Vector2 padding)
{
	Vector2 worldSize = g_pGameManager->Renderer()->GetViewSize() * Fixed::OneHalf;
	if (outPosition.x + padding.x > worldSize.x)
	{
		outPosition.x = (worldSize.x - padding.x);
	}
	else if (outPosition.x - padding.x < -worldSize.x)
	{
		outPosition.x = -(worldSize.x - padding.x);
	}
	if (outPosition.y + padding.y > worldSize.y)
	{
		outPosition.y = (worldSize.y - padding.y);
	}
	else if (outPosition.y - padding.y < -worldSize.y)
	{
		outPosition.y = -(worldSize.y - padding.y);
	}
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
	SetName("Controller");
	m_pRenderComponent = m_pOwner->GetComponent<RenderComponent>();
	if (!m_pRenderComponent)
	{
		LOG_E("%s : %s has ControllerComponent but no RenderComponent!", LogNameStr(), m_pOwner->LogNameStr());
	}
	BindInput([&](const LEInput::Frame& x) -> bool { return OnInput(x); });
	Reset();
}

void ControllerComponent::Tick(Time dt)
{
	if (Maths::Abs(m_rotation) > Fixed::Zero)
	{
		m_pOwner->m_transform.Rotate(m_rotation * m_angularSpeed * dt.AsMilliseconds());
	}
	if (m_displacement.SqrMagnitude() > 0.0)
	{
		m_pOwner->m_transform.localPosition += (m_displacement * m_linearSpeed * dt.AsMilliseconds());
	}
	if (m_pRenderComponent)
	{
		ClampPosition(m_pOwner->m_transform.localPosition,
					  m_pRenderComponent->m_pSFPrimitive->GetBounds().GetSize() * Fixed::OneHalf);
	}
}

void ControllerComponent::SetEnabled(bool bEnabled)
{
	Super::SetEnabled(bEnabled);
	Reset();
}

bool ControllerComponent::OnInput(const LEInput::Frame& frame)
{
	if (!m_bEnabled)
	{
		return false;
	}

	bool bModifier = frame.IsHeld(GameInputType::LB);
	m_rotation = Fixed::Zero;
	m_displacement = Vector2::Zero;

	// Horizontal
	{
		if (frame.IsHeld(GameInputType::Left))
		{
			if (bModifier)
			{
				m_rotation = 1;
			}
			else
			{
				m_displacement.x = -1;
			}
		}

		else if (frame.IsHeld(GameInputType::Right))
		{
			if (bModifier)
			{
				m_rotation = -1;
			}
			else
			{
				m_displacement.x = 1;
			}
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

	if (m_displacement.SqrMagnitude() > 0.0)
	{
		m_displacement.Normalise();
	}
	return false;
}
} // namespace LittleEngine
