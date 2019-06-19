#include "stdafx.h"
#include "Core/Logger.h"
#include "SFMLAPI/Input/SFInputDataFrame.h"
#include "SFMLAPI/Input/SFInputStateMachine.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "LittleEngine/Debug/Tweakable.h"
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

#if DEBUGGING
bool ControllerComponent::s_bShowJoystickOrientation = false;
TweakBool(joystickOrientation, &ControllerComponent::s_bShowJoystickOrientation);
#endif

ControllerComponent::ControllerComponent() = default;

ControllerComponent::~ControllerComponent()
{
#if DEBUGGING
	if (m_pRect)
	{
		m_pRect->Destroy();
	}
#endif
}

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
	BindInput([&](const LEInput::Frame& x) -> bool { return OnInput(x); }, true);
	Reset();

#if DEBUGGING
	m_pRect = g_pGameManager->Renderer()->New<SFRect>(LAYER_TOP);
	m_pRect->SetSize({200, 3})->SetPivot({-1, 0})->SetPrimaryColour(Colour::Magenta)->SetEnabled(s_bShowJoystickOrientation);
#endif
}

void ControllerComponent::Tick(Time dt)
{
	Transform& t = m_pOwner->m_transform;
	if (Maths::Abs(m_rotation) > Fixed::Zero)
	{
		Fixed orientation = Vector2::ToOrientation(t.GetOrientation()) +
							m_rotation * m_angularSpeed * dt.AsMilliseconds();
		t.SetOrientation(orientation);
	}
	if (m_displacement.SqrMagnitude() > 0.0)
	{
		Vector2 pos = t.GetPosition() + (m_displacement * m_linearSpeed * dt.AsMilliseconds());
		if (m_pRenderComponent)
		{
			ClampPosition(pos,
						  m_pRenderComponent->m_pPrimitive->GetBounds().GetSize() * Fixed::OneHalf);
		}
		t.SetPosition(pos);
	}
#if DEBUGGING
	m_pRect->SetPosition(m_pOwner->m_transform.GetWorldPosition());
	m_pRect->SetEnabled(s_bShowJoystickOrientation);
#endif
}

void ControllerComponent::SetEnabled(bool bEnabled)
{
	Super::SetEnabled(bEnabled);
#if DEBUGGING
	if (m_pRect)
	{
		m_pRect->SetEnabled(s_bShowJoystickOrientation && bEnabled);
	}
#endif
	Reset();
}

bool ControllerComponent::OnInput(const LEInput::Frame& frame)
{
	m_bKeyInput = false;
	if (!m_bEnabled)
	{
		return false;
	}

	bool bModifier = frame.IsHeld(KeyCode::LControl) || frame.IsHeld(KeyCode::RControl) ||
					 frame.IsHeld(KeyType::JOY_BTN_4);
	m_rotation = Fixed::Zero;
	m_displacement = Vector2::Zero;

	// Horizontal
	{
		if (frame.IsHeld(KeyCode::Left))
		{
			m_bKeyInput = true;
			if (bModifier)
			{
				m_rotation = 1;
			}
			else
			{
				m_displacement.x = -1;
			}
		}

		else if (frame.IsHeld(KeyCode::Right))
		{
			m_bKeyInput = true;
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
		if (frame.IsHeld(KeyCode::Up))
		{
			m_bKeyInput = true;
			m_displacement.y += 1;
		}

		else if (frame.IsHeld(KeyCode::Down))
		{
			m_bKeyInput = true;
			m_displacement.y -= 1;
		}
	}

	if (m_displacement.SqrMagnitude() > 0.0)
	{
		m_displacement.Normalise();
	}

	for (const auto& state : frame.joyInput.m_states)
	{
		if (!m_bKeyInput)
		{
			m_displacement = Vector2::Zero;
			m_rotation = Fixed::Zero;
		}
		if (Maths::Abs(state.xy.x) > SFInputStateMachine::JOY_DEADZONE)
		{
			m_displacement.x += (state.xy.x / Fixed(100));
		}
		if (Maths::Abs(state.xy.y) > SFInputStateMachine::JOY_DEADZONE)
		{
			m_displacement.y += (state.xy.y / Fixed(100));
		}
		if (Maths::Abs(state.uv.x) > SFInputStateMachine::JOY_DEADZONE)
		{
			m_rotation -= (state.uv.x / Fixed(100));
		}
		if (Maths::Abs(state.uv.SqrMagnitude()) > 0.5f)
		{
			Vector2 t = state.uv.Normalised();
			Vector2 s = m_pOwner->m_transform.GetWorldOrientation();
			Vector2 s90 = {s.x.Cos() * s.y.Sin(), -s.x.Sin() * s.y.Cos()};
			Fixed proj = s.Dot(t);
			Fixed dir = s90.Dot(t);
			if (Maths::Abs(proj) < Fixed(0.975f))
			{
				if ((proj > Fixed::Zero && dir > Fixed::Zero) || (proj < Fixed::Zero && dir > Fixed::Zero))
				{
					m_rotation = -Fixed::One;
				}
				else if ((proj > Fixed::Zero && dir < Fixed::Zero) || (proj < Fixed::Zero && dir < Fixed::Zero))
				{
					m_rotation = Fixed::One;
				}
			}
			else
			{
				m_rotation = Fixed::Zero;
				m_pOwner->m_transform.SetOrientation(t);
			}
#if DEBUGGING
			if (m_pRect)
			{
				m_pRect->SetOrientation(t);
			}
#endif
		}
	}
	return false;
}
} // namespace LittleEngine
