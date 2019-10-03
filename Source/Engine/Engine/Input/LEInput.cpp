#include "Core/Logger.h"
#include "Core/Utils.h"
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "LEInput.h"
#include "Engine/Context/LEContext.h"
#if defined(DEBUGGING)
#include "Engine/GFX.h"
#include "Engine/Debug/Tweakable.h"
#include "Engine/Rendering/LERenderer.h"
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

LEInput::LEInput(InputMap inputMap)
{
	m_inputSM.SetInputMapping(std::move(inputMap));
}

LEInput::~LEInput() = default;

Token LEInput::Register(Delegate callback, bool bForce)
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

Token LEInput::RegisterSudo(Delegate callback)
{
	Token token = CreateToken();
	InputContext sudo{std::move(callback), token, true};
	m_oSudoContext.emplace(sudo);
	return token;
}

void LEInput::TakeSnapshot()
{
	auto uniqueInsert = [&](s32 toInsert) {
		if (toInsert >= 0)
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
	Colour c = MOUSE_DEFAULT_COLOUR;
	if (Core::Search(m_currentSnapshot, KeyType::MOUSE_BTN_0) != m_currentSnapshot.end())
	{
		c = MOUSE_LEFT_COLOUR;
	}
	if (Core::Search(m_currentSnapshot, KeyType::MOUSE_BTN_1) != m_currentSnapshot.end())
	{
		c = MOUSE_RIGHT_COLOUR;
	}

	Vector2 pos = g_pGFX->WorldToOverlay(m_mouseInput.worldPosition);
	if (m_pMouseH)
	{
		m_pMouseH->SetPosition({pos.x, 0})->SetEnabled(bShowCrosshair)->SetColour(c);
	}
	if (m_pMouseV)
	{
		m_pMouseV->SetPosition({0, pos.y})->SetEnabled(bShowCrosshair)->SetColour(c);
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
		LOG_D("[Input] Deleted [%d] stale registrants", prev - curr);
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
void LEInput::CreateDebugPointer(LERenderer& renderer)
{
	m_pMouseH = renderer.New<Quad>(LayerID::TopOverlay);
	m_pMouseV = renderer.New<Quad>(LayerID::TopOverlay);
	Vector2 space = g_pGFX->OverlaySpace();
	m_pMouseH->SetModel(Rect2::SizeCentre({MOUSE_QUAD_WIDTH, space.y}), true)
		->SetStatic(true)
		->SetColour(MOUSE_DEFAULT_COLOUR)
		->SetEnabled(true);
	m_pMouseV->SetModel(Rect2::SizeCentre({space.x, MOUSE_QUAD_WIDTH}), true)
		->SetStatic(true)
		->SetColour(MOUSE_DEFAULT_COLOUR)
		->SetEnabled(true);
}
#endif

Token LEInput::CreateToken() const
{
	return MakeToken(m_contexts.size() + 1);
}
} // namespace LittleEngine
