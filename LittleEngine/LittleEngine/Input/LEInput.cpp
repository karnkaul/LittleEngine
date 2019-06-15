#include "stdafx.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "SFMLAPI/Input/SFInputStateMachine.h"
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "LEInput.h"
#include "LittleEngine/Context/LEContext.h"
#if DEBUGGING
#include "LittleEngine/Debug/Tweakable.h"
#include "LittleEngine/Renderer/LERenderer.h"
#endif

namespace LittleEngine
{
#if DEBUGGING
namespace
{
const Colour MOUSE_LEFT_COLOUR = Colour(230, 0, 100, 255);
const Colour MOUSE_RIGHT_COLOUR = Colour(0, 230, 100, 255);
const Colour MOUSE_DEFAULT_COLOUR = Colour(100, 100, 100, 255);
const u8 MOUSE_QUAD_WIDTH = 2;
bool bShowCrosshair = false;
} // namespace
#endif

String LEInput::Frame::GetClipboard()
{
	return SFInputDataFrame::GetClipboard();
}

LEInput::Frame::Frame(Vec<GameInputType> pressed, Vec<GameInputType> held, Vec<GameInputType> released, TextInput textInput)
	: pressed(std::move(pressed)),
	  held(std::move(held)),
	  released(std::move(released)),
	  textInput(std::move(textInput))
{
}

bool LEInput::Frame::IsPressed(GameInputType keyCode) const
{
	return Core::Search(pressed, keyCode) != pressed.end();
}

bool LEInput::Frame::IsHeld(GameInputType keyCode) const
{
	return Core::Search(held, keyCode) != held.end();
}

bool LEInput::Frame::IsReleased(GameInputType keyCode) const
{
	return Core::Search(released, keyCode) != released.end();
}

bool LEInput::Frame::HasData() const
{
	return !pressed.empty() || !held.empty() || !released.empty() || !textInput.specials.empty() ||
		   !textInput.text.empty();
}

LEInput::InputContext::InputContext(Delegate callback, Token& sToken)
	: callback(std::move(callback)), wToken(sToken)
{
}

#if DEBUGGING
TweakBool(mouseXY, &bShowCrosshair);
#endif

LEInput::LEInput(LEContext& context) : m_pContext(&context)
{
	BindDefaults();
}

LEInput::~LEInput() = default;

LEInput::Token LEInput::Register(Delegate callback)
{
	Token token = CreateToken();
	InputContext newTop(std::move(callback), token);
	m_contexts.emplace_back(newTop);
	return token;
}

MouseInput LEInput::GetMouseState() const
{
	return m_mouseInput;
}

LEInput::Token LEInput::RegisterSudo(Delegate callback)
{
	Token token = CreateToken();
	m_uSudoContext = MakeUnique<InputContext>(std::move(callback), token);
	return token;
}

void LEInput::TakeSnapshot(const SFInputDataFrame& frameData)
{
	m_previousSnapshot = m_currentSnapshot;
	m_textInput = frameData.textInput;
	m_mouseInput = frameData.mouseInput;
	m_currentSnapshot.clear();
	for (const auto& key : frameData.pressed)
	{
		GameInputType input = m_gamepad.ToGameInputType(key);
		if (input != GameInputType::Invalid)
		{
			auto duplicate = Core::Search(m_currentSnapshot, input);
			if (duplicate == m_currentSnapshot.end())
			{
				m_currentSnapshot.push_back(input);
			}
		}
	}
#if DEBUGGING
	if (bShowCrosshair && !m_pMouseH)
	{
		CreateDebugPointer();
	}
	Colour c = m_mouseInput.bLeftPressed
				   ? MOUSE_LEFT_COLOUR
				   : m_mouseInput.bRightPressed ? MOUSE_RIGHT_COLOUR : MOUSE_DEFAULT_COLOUR;
	if (m_pMouseH)
	{
		m_pMouseH->SetPosition({m_mouseInput.worldPosition.x, 0})->SetEnabled(bShowCrosshair)->SetPrimaryColour(c);
	}
	if (m_pMouseV)
	{
		m_pMouseV->SetPosition({0, m_mouseInput.worldPosition.y})->SetEnabled(bShowCrosshair)->SetPrimaryColour(c);
	}
#endif
}

void LEInput::FireCallbacks()
{
	Vec<GameInputType> pressed, held, released;

	// Build "pressed" and "held" vectors
	for (auto input : m_currentSnapshot)
	{
		auto search = Core::Search(m_previousSnapshot, input);
		if (search != m_previousSnapshot.end())
		{
			held.push_back(input);
		}
		else
		{
			pressed.push_back(input);
		}
	}

	released = m_previousSnapshot;
	Core::RemoveIf<GameInputType>(released, [&held](GameInputType type) {
		return Core::Search(held, type) != held.end();
	});
	if (m_uSudoContext && m_uSudoContext->wToken.expired())
	{
		m_uSudoContext = nullptr;
	}

	Frame dataFrame(pressed, held, released, m_textInput);
	if (dataFrame.HasData())
	{
		size_t prev = m_contexts.size();
		Core::RemoveIf<InputContext>(
			m_contexts, [](InputContext& context) { return context.wToken.expired(); });
		size_t curr = m_contexts.size();
		if (curr != prev)
		{
			LOG_D("[Input] Deleted %d stale contexts", prev - curr);
		}

		if (m_uSudoContext)
		{
			if (m_uSudoContext->callback(dataFrame))
			{
				return;
			}
		}

		for (auto iter = m_contexts.rbegin(); iter != m_contexts.rend(); ++iter)
		{
			if (iter->callback(dataFrame))
			{
				return;
			}
		}
	}
}

void LEInput::BindDefaults()
{
	m_gamepad.Bind(GameInputType::Up, {KeyCode::Up, KeyCode::W});
	m_gamepad.Bind(GameInputType::Down, {KeyCode::Down, KeyCode::S});
	m_gamepad.Bind(GameInputType::Left, {KeyCode::Left, KeyCode::A});
	m_gamepad.Bind(GameInputType::Right, {KeyCode::Right, KeyCode::D});
	m_gamepad.Bind(GameInputType::Enter, KeyCode::Enter);
	m_gamepad.Bind(GameInputType::Back, KeyCode::Escape);
	m_gamepad.Bind(GameInputType::Select, KeyCode::Tab);

	m_gamepad.Bind(GameInputType::X, KeyCode::Space);
	m_gamepad.Bind(GameInputType::Y, KeyCode::E);
	m_gamepad.Bind(GameInputType::A, KeyCode::R);
	m_gamepad.Bind(GameInputType::B, KeyCode::F);
	m_gamepad.Bind(GameInputType::LB, {KeyCode::LControl, KeyCode::RControl});
	m_gamepad.Bind(GameInputType::RB, {KeyCode::LShift, KeyCode::RShift});

	m_gamepad.Bind(GameInputType::Debug0, KeyCode::F12);
}

#if DEBUGGING
void LEInput::CreateDebugPointer()
{
	Vector2 viewSize = m_pContext->GetViewSize();
	m_pMouseH = m_pContext->Renderer()->New<Quad>(LAYER_TOP);
	m_pMouseV = m_pContext->Renderer()->New<Quad>(LAYER_TOP);
	m_pMouseH->SetModel(Rect2::CentreSize({MOUSE_QUAD_WIDTH, viewSize.y}))
		->SetStatic(true)
		->SetPrimaryColour(MOUSE_DEFAULT_COLOUR)
		->SetEnabled(true);
	m_pMouseV->SetModel(Rect2::CentreSize({viewSize.x, MOUSE_QUAD_WIDTH}))
		->SetStatic(true)
		->SetPrimaryColour(MOUSE_DEFAULT_COLOUR)
		->SetEnabled(true);
}
#endif

LEInput::Token LEInput::CreateToken() const
{
	return MakeShared<s32>(m_contexts.size() + 1);
}
} // namespace LittleEngine
