#include <bitset>
#include "Core/Logger.h"
#include "SFMLAPI/Input/InputMappings.h"
#include "SFMLAPI/Input/InputStateMachine.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "Engine/GFX.h"
#include "Engine/Context/LEContext.h"
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
void ClampPosition(Vector2& outPosition, Vector2 size)
{
	Vector2 world = g_pGFX->WorldSpace();
	Fixed maxX = Fixed::OneHalf * (world.x - size.x);
	Fixed maxY = Fixed::OneHalf * (world.y - size.y);
	outPosition.x = Maths::Clamp(outPosition.x, -maxX, maxX);
	outPosition.y = Maths::Clamp(outPosition.y, -maxY, maxY);
}
} // namespace

#if defined(DEBUGGING)
bool ControllerComponent::s_bShowJoystickOrientation = false;
Array<Colour, 2> ControllerComponent::s_ornColours = {Colour(255, 0, 255), Colour(150, 150, 150)};
Vector2 ControllerComponent::s_orientationWidthHeight = {120, 3};
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
	m_pRect->SetSize(s_orientationWidthHeight)->SetPivot({-1, 0})->SetEnabled(s_bShowJoystickOrientation);
#endif
	m_ptrToken = g_pGameManager->Context()->PushPointer(LEContext::Pointer::Type::Cross);
}

void ControllerComponent::Tick(Time dt)
{
	if (g_pGameManager->IsPlayerControllable())
	{
		Transform& t = m_pOwner->m_transform;
		Vector2 orn = t.Orientation();
		if (orn != m_targetOrn)
		{
			Vector2 newOrn = Maths::Lerp(orn, m_targetOrn, m_angularSpeed * dt.AsMilliseconds() / 40);
			t.SetOrientation(newOrn);
		}
		if (m_displacement.SqrMagnitude() > 0.0)
		{
			Vector2 pos = t.Position() + (m_displacement * m_linearSpeed * dt.AsMilliseconds());
			if (m_pRenderComponent)
			{
				ClampPosition(pos, m_pRenderComponent->m_pPrimitive->RenderBounds().Size());
			}
			t.SetPosition(pos);
			m_displacement = Vector2::Zero;
		}
	}
#if defined(DEBUGGING)
	m_pRect->SetEnabled(s_bShowJoystickOrientation);
	if (s_bShowJoystickOrientation)
	{
		Colour c = s_ornColours[ToIdx(m_active)];
		m_pRect->SetPosition(m_pOwner->WorldMatrix().Position())->SetColour(c);
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
	m_targetOrn = Vector2::Up;
}

bool ControllerComponent::OnInput(const LEInput::Frame& frame)
{
	if (m_bEnabled)
	{
		std::bitset<2> current;
		bool bKeyPressed =
			frame.IsHeld({KeyCode::Down, KeyCode::Up, KeyCode::Left, KeyCode::Right, KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D});
		bool bMouseChanged = frame.mouseInput.worldPosition != m_prevMousePos;
		current[ToIdx(Scheme::KBM)] = bKeyPressed || bMouseChanged;
		const JoyState* pJoyState = nullptr;
		for (const auto& state : frame.joyInput.m_states)
		{
			if (Maths::Abs(state.xy.SqrMagnitude()) > s_XY_DEADZONE || Maths::Abs(state.uv.SqrMagnitude()) > s_ORN_DEADZONE)
			{
				pJoyState = &state;
				current[ToIdx(Scheme::Joystick)] = true;
				break;
			}
		}
		if (current.any())
		{
			if (current[ToIdx(Scheme::Joystick)] && pJoyState)
			{
				m_active = Scheme::Joystick;
				m_displacement.x += pJoyState->xy.x;
				m_displacement.y += pJoyState->xy.y;
				m_targetOrn = pJoyState->uv.Normalised();
#if defined(DEBUGGING)
				if (m_pRect && s_bShowJoystickOrientation)
				{
					m_pRect->SetOrientation(m_targetOrn);
				}
#endif
			}
			else
			{
				m_active = Scheme::KBM;
				Vector2 orn = frame.mouseInput.worldPosition - m_pOwner->m_transform.WorldPosition();
				m_targetOrn = orn.Normalised();
				if (frame.IsHeld({KeyCode::Left, KeyCode::A}))
				{
					m_displacement.x += -1;
				}
				if (frame.IsHeld({KeyCode::Right, KeyCode::D}))
				{
					m_displacement.x += 1;
				}
				if (frame.IsHeld({KeyCode::Up, KeyCode::W}))
				{
					m_displacement.y += 1;
				}
				if (frame.IsHeld({KeyCode::Down, KeyCode::S}))
				{
					m_displacement.y += -1;
				}
			}
		}
		m_prevMousePos = frame.mouseInput.worldPosition;
	}
	return false;
}
} // namespace LittleEngine
