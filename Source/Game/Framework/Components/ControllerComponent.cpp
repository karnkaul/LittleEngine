#include "Core/Logger.h"
#include "SFMLAPI/Input/InputMappings.h"
#include "SFMLAPI/Input/InputStateMachine.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "Engine/GFX.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Renderer/LERenderer.h"
#include "Model/World/Entity.h"
#include "Model/GameManager.h"
#include "ControllerComponent.h"
#include "RenderComponent.h"

namespace LittleEngine
{
namespace
{
void ClampPosition(Vector2& outPosition, Vector2 padding)
{
	Vector2 halfSize = g_pGFX->WorldSpace() * Fixed::OneHalf;
	if (outPosition.x + padding.x > halfSize.x)
	{
		outPosition.x = (halfSize.x - padding.x);
	}
	else if (outPosition.x - padding.x < -halfSize.x)
	{
		outPosition.x = -(halfSize.x - padding.x);
	}
	if (outPosition.y + padding.y > halfSize.y)
	{
		outPosition.y = (halfSize.y - padding.y);
	}
	else if (outPosition.y - padding.y < -halfSize.y)
	{
		outPosition.y = -(halfSize.y - padding.y);
	}
}
} // namespace

#if defined(DEBUGGING)
bool ControllerComponent::s_bShowJoystickOrientation = false;
Vector2 ControllerComponent::s_orientationWidthHeight = {120, 3};
Colour ControllerComponent::s_orientationColour = Colour(255, 0, 255);
TweakBool(joystickOrientation, &ControllerComponent::s_bShowJoystickOrientation);
#endif

Fixed ControllerComponent::s_orientationEpsilon = Fixed(0.1f);

ControllerComponent::ControllerComponent() = default;

ControllerComponent::~ControllerComponent()
{
#if defined(DEBUGGING)
	if (m_pRect)
	{
		m_pRect->Destroy();
	}
#endif
}

void ControllerComponent::OnCreated()
{
	SetName("Controller");
	m_pRenderComponent = m_pOwner->GetComponent<RenderComponent>();
	if (!m_pRenderComponent)
	{
		LOG_E("%s : %s has ControllerComponent but no RenderComponent!", m_logName.data(), m_pOwner->LogName().data());
	}
	BindInput([&](const LEInput::Frame& x) -> bool { return OnInput(x); }, true);
	Reset();

#if defined(DEBUGGING)
	m_pRect = g_pGameManager->Renderer()->New<SFRect>(LayerID::DebugWorld);
	m_pRect->SetSize(s_orientationWidthHeight)
		->SetPivot({-1, 0})
		->SetColour(s_orientationColour)
		->SetEnabled(s_bShowJoystickOrientation);
#endif
}

void ControllerComponent::Tick(Time dt)
{
	if (g_pGameManager->IsPlayerControllable())
	{
		Transform& t = m_pOwner->m_transform;
		if (Maths::Abs(m_rotation) > Fixed::Zero)
		{
			Fixed orientation = Vector2::ToOrientation(t.Orientation()) + m_rotation * m_angularSpeed * dt.AsMilliseconds();
			t.SetOrientation(orientation);
		}
		if (m_displacement.SqrMagnitude() > 0.0)
		{
			Vector2 pos = t.Position() + (m_displacement * m_linearSpeed * dt.AsMilliseconds());
			if (m_pRenderComponent)
			{
				ClampPosition(pos, m_pRenderComponent->m_pPrimitive->RenderBounds().Size() * Fixed::OneHalf);
			}
			t.SetPosition(pos);
		}
	}
#if defined(DEBUGGING)
	m_pRect->SetEnabled(s_bShowJoystickOrientation);
	if (s_bShowJoystickOrientation)
	{
		m_pRect->SetPosition(m_pOwner->WorldMatrix().Position());
	}
#endif
}

TimingType ControllerComponent::Timing() const
{
	return TimingType::Pre;
}

void ControllerComponent::SetEnabled(bool bEnabled)
{
	Super::SetEnabled(bEnabled);
#if defined(DEBUGGING)
	if (m_pRect)
	{
		m_pRect->SetEnabled(s_bShowJoystickOrientation && bEnabled);
	}
#endif
	Reset();
}

void ControllerComponent::Reset()
{
	m_displacement = Vector2::Zero;
	m_rotation = Fixed::Zero;
}

bool ControllerComponent::OnInput(const LEInput::Frame& frame)
{
	m_bKeyInput = false;
	if (!m_bEnabled)
	{
		return false;
	}

	bool bModifier = frame.IsHeld(KeyCode::LControl) || frame.IsHeld(KeyCode::RControl) || frame.IsHeld(KeyType::JOY_BTN_4);
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
		m_displacement.x += state.xy.x;
		m_displacement.y += state.xy.y;
		if (Maths::Abs(state.uv.SqrMagnitude()) > 0.5f)
		{
			Vector2 t = state.uv.Normalised();
			Vector2 s = m_pOwner->m_transform.WorldOrientation();
			Vector2 s90 = {s.x.Cos() * s.y.Sin(), -s.x.Sin() * s.y.Cos()};
			Fixed proj = s.Dot(t);
			Fixed dir = s90.Dot(t);
			Fixed one = Fixed::One - s_orientationEpsilon;
			if (Maths::Abs(proj) < one || proj < -one)
			{
				if ((proj > Fixed::Zero && dir > Fixed::Zero) || (proj <= Fixed::Zero && dir >= Fixed::Zero))
				{
					m_rotation = -Fixed::One;
				}
				else if ((proj > Fixed::Zero && dir < Fixed::Zero) || (proj <= Fixed::Zero && dir <= Fixed::Zero))
				{
					m_rotation = Fixed::One;
				}
			}
			else
			{
				LOG_D("projection: %.2f", proj.ToF32());
				m_rotation = Fixed::Zero;
				m_pOwner->m_transform.SetOrientation(t);
			}
#if defined(DEBUGGING)
			if (m_pRect && s_bShowJoystickOrientation)
			{
				m_pRect->SetOrientation(t);
			}
#endif
		}
	}
	return false;
}
} // namespace LittleEngine
