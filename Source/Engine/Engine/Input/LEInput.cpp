#include "Core/Logger.h"
#include "Core/Utils.h"
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "LEInput.h"
#include "Engine/Context/LEContext.h"
#if defined(DEBUGGING)
#include "Engine/Debug/Tweakable.h"
#include "Engine/Renderer/LERenderer.h"
#endif

namespace LittleEngine
{
#if defined(DEBUGGING)
namespace
{
const Colour MOUSE_LEFT_COLOUR = Colour(230, 0, 100, 255);
const Colour MOUSE_RIGHT_COLOUR = Colour(0, 230, 100, 255);
const Colour MOUSE_DEFAULT_COLOUR = Colour(100, 100, 100, 255);
const u8 MOUSE_QUAD_WIDTH = 2;
bool bShowCrosshair = false;
} // namespace
#endif

String LEInput::Frame::Clipboard()
{
	return InputDataFrame::Clipboard();
}

bool LEInput::Frame::IsPressed(s32 keyCode) const
{
	return Core::Search(pressed, static_cast<KeyType>(keyCode)) != pressed.end();
}

bool LEInput::Frame::IsHeld(s32 keyCode) const
{
	return Core::Search(held, static_cast<KeyType>(keyCode)) != held.end();
}

bool LEInput::Frame::IsReleased(s32 keyCode) const
{
	return Core::Search(released, static_cast<KeyType>(keyCode)) != released.end();
}

bool LEInput::Frame::IsPressed(InitList<s32> keys, bool bAny /* = true */) const
{
	return Result(
		keys, [&](s32 key) { return IsPressed(key); }, bAny);
}

bool LEInput::Frame::IsHeld(InitList<s32> keys, bool bAny /* = true */) const
{
	return Result(
		keys, [&](s32 key) { return IsHeld(key); }, bAny);
}

bool LEInput::Frame::IsReleased(InitList<s32> keys, bool bAny /* = true */) const
{
	return Result(
		keys, [&](s32 key) { return IsReleased(key); }, bAny);
}

bool LEInput::Frame::HasData() const
{
	return !pressed.empty() || !held.empty() || !released.empty();
}

Fixed LEInput::Frame::MouseWhellScroll() const
{
	return mouseInput.scrollDelta;
}

#if defined(DEBUGGING)
TweakBool(mouseXY, &bShowCrosshair);
#endif

LEInput::LEInput(LEContext& context, InputMap inputMap) : m_pContext(&context)
{
	m_inputSM.SetInputMapping(std::move(inputMap));
}

LEInput::~LEInput() = default;

LEInput::Token LEInput::Register(Delegate callback, bool bForce)
{
	Token token = CreateToken();
	InputContext newTop{std::move(callback), token, bForce};
	m_contexts.emplace_back(newTop);
	return token;
}

MouseInput LEInput::MouseState() const
{
	return m_mouseInput;
}

LEInput::Token LEInput::RegisterSudo(Delegate callback)
{
	Token token = CreateToken();
	InputContext sudo{std::move(callback), token, true};
	m_oSudoContext.emplace(sudo);
	return token;
}

void LEInput::TakeSnapshot()
{
	auto uniqueInsert = [&](s32 toInsert) {
		if (toInsert > 0)
		{
			KeyType key = static_cast<KeyType>(toInsert);
			if (Core::Search(m_currentSnapshot, key) == m_currentSnapshot.end())
			{
				m_currentSnapshot.push_back(key);
			}
		}
	};
	auto frameData = m_inputSM.Dataframe();
	m_previousSnapshot = m_currentSnapshot;
	m_textInput = frameData.textInput;
	m_mouseInput = frameData.mouseInput;
	m_joyInput = frameData.joyInput;
	m_currentSnapshot.clear();
	for (const auto& key : frameData.pressed)
	{
		uniqueInsert(key.GetKeyType());
	}
	for (const auto& state : frameData.joyInput.m_states)
	{
		// POV
		if (state.pov.x > Fixed::OneHalf)
		{
			uniqueInsert(KeyCode::Right);
		}
		if (state.pov.x < -Fixed::OneHalf)
		{
			uniqueInsert(KeyCode::Left);
		}
		if (state.pov.y > Fixed::OneHalf)
		{
			uniqueInsert(KeyCode::Up);
		}
		if (state.pov.y < -Fixed::OneHalf)
		{
			uniqueInsert(KeyCode::Down);
		}
	}
#if defined(DEBUGGING)
	if (bShowCrosshair && !m_pMouseH)
	{
		CreateDebugPointer();
	}
	Colour c = MOUSE_DEFAULT_COLOUR;
	if (Core::Search(m_currentSnapshot, KeyType::MOUSE_BTN_0) != m_currentSnapshot.end())
	{
		c = MOUSE_LEFT_COLOUR;
	}
	if (Core::Search(m_currentSnapshot, KeyType::MOUSE_BTN_1) != m_currentSnapshot.end())
	{
		c = MOUSE_RIGHT_COLOUR;
	}

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
	Vec<KeyType> pressed;
	Vec<KeyType> held;
	Vec<KeyType> released;

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
	Core::RemoveIf<KeyType>(released, [&held](KeyType type) { return Core::Search(held, type) != held.end(); });
	if (m_oSudoContext && m_oSudoContext->wToken.expired())
	{
		m_oSudoContext.reset();
	}

	Frame dataFrame{pressed, held, released, m_textInput, m_mouseInput, m_joyInput};
	bool bHasData = dataFrame.HasData();
	size_t prev = m_contexts.size();
	Core::RemoveIf<InputContext>(m_contexts, [](const InputContext& context) { return context.wToken.expired(); });
	size_t curr = m_contexts.size();
	if (curr != prev)
	{
		LOG_D("[Input] Deleted %d stale contexts", prev - curr);
	}

	if (m_oSudoContext)
	{
		if (m_oSudoContext->callback(dataFrame))
		{
			return;
		}
	}

	for (auto iter = m_contexts.rbegin(); iter != m_contexts.rend(); ++iter)
	{
		if (bHasData || iter->bForce)
		{
			if (iter->callback(dataFrame))
			{
				return;
			}
		}
	}
}

#if defined(DEBUGGING)
void LEInput::CreateDebugPointer()
{
	Vector2 viewSize = m_pContext->ViewSize();
	m_pMouseH = m_pContext->Renderer()->New<Quad>(LayerID::Top);
	m_pMouseV = m_pContext->Renderer()->New<Quad>(LayerID::Top);
	m_pMouseH->SetModel(Rect2::SizeCentre({MOUSE_QUAD_WIDTH, viewSize.y}))
		->SetStatic(true)
		->SetPrimaryColour(MOUSE_DEFAULT_COLOUR)
		->SetEnabled(true);
	m_pMouseV->SetModel(Rect2::SizeCentre({viewSize.x, MOUSE_QUAD_WIDTH}))
		->SetStatic(true)
		->SetPrimaryColour(MOUSE_DEFAULT_COLOUR)
		->SetEnabled(true);
}
#endif

LEInput::Token LEInput::CreateToken() const
{
	return MakeShared<s32>(static_cast<s32>(m_contexts.size() + 1));
}
} // namespace LittleEngine
