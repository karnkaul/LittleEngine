#include "stdafx.h"
#include "EngineInput.h"
#include "Logger.h"
#include "Utils.h"

namespace LittleEngine
{
EngineInput::Frame::Frame(const Vector<GameInputType>& pressed,
						  const Vector<GameInputType>& held,
						  const Vector<GameInputType>& released,
						  const TextInput& textInput)
	: pressed(pressed), held(held), released(released), textInput(textInput)
{
}

bool EngineInput::Frame::IsPressed(GameInputType keyCode) const
{
	return Core::VectorSearch(pressed, keyCode) != pressed.end();
}

bool EngineInput::Frame::IsHeld(GameInputType keyCode) const
{
	return Core::VectorSearch(held, keyCode) != held.end();
}

bool EngineInput::Frame::IsReleased(GameInputType keyCode) const
{
	return Core::VectorSearch(released, keyCode) != released.end();
}

bool EngineInput::Frame::HasData() const
{
	return !pressed.empty() || !held.empty() || !released.empty() || !textInput.specials.empty() ||
		   !textInput.text.empty();
}

EngineInput::InputContext::InputContext(Delegate Callback, Token& sToken)
	: Callback(Callback), wToken(sToken)
{
}

EngineInput::EngineInput()
{
	BindDefaults();
}

EngineInput::Token EngineInput::Register(Delegate Callback)
{
	Token token = CreateToken();
	InputContext newTop(Callback, token);
	m_contexts.emplace_back(newTop);
	return token;
}

EngineInput::Token EngineInput::RegisterSudo(Delegate Callback)
{
	Token token = CreateToken();
	m_uSudoContext = MakeUnique<InputContext>(Callback, token);
	return token;
}

void EngineInput::TakeSnapshot(const SFInputDataFrame& frameData)
{
	m_previousSnapshot = m_currentSnapshot;
	m_textInput = frameData.textInput;
	m_currentSnapshot.clear();
	for (const auto& key : frameData.pressed)
	{
		GameInputType input = m_gamepad.ToGameInputType(key);
		if (input != GameInputType::Invalid)
		{
			auto duplicate = Core::VectorSearch(m_currentSnapshot, input);
			if (duplicate == m_currentSnapshot.end())
				m_currentSnapshot.push_back(input);
		}
	}
}

void EngineInput::FireCallbacks()
{
	Vector<GameInputType> pressed, held, released;

	// Build "pressed" and "held" vectors
	for (auto input : m_currentSnapshot)
	{
		auto search = Core::VectorSearch(m_previousSnapshot, input);
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
	Core::CleanVector<GameInputType>(released, [&held](GameInputType type) {
		return Core::VectorSearch(held, type) != held.end();
	});
	if (m_uSudoContext && m_uSudoContext->wToken.expired())
		m_uSudoContext = nullptr;

	Frame dataFrame(pressed, held, released, m_textInput);
	if (dataFrame.HasData())
	{
		size_t prev = m_contexts.size();
		Core::CleanVector<InputContext>(
			m_contexts, [](InputContext& context) { return context.wToken.expired(); });
		size_t curr = m_contexts.size();
		if (curr != prev)
			LOG_D("[EngineInput] Deleted %d stale contexts", prev - curr);

		if (m_uSudoContext)
		{
			if (m_uSudoContext->Callback(dataFrame))
			{
				return;
			}
		}

		for (auto iter = m_contexts.rbegin(); iter != m_contexts.rend(); ++iter)
		{
			if (iter->Callback(dataFrame))
			{
				return;
			}
		}
	}
}

void EngineInput::BindDefaults()
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
	m_gamepad.Bind(GameInputType::LB, {KeyCode::LControl, KeyCode::RControl});
	m_gamepad.Bind(GameInputType::RB, {KeyCode::LShift, KeyCode::RShift});

	m_gamepad.Bind(GameInputType::Debug0, KeyCode::F12);
}

EngineInput::Token EngineInput::CreateToken() const
{
	return MakeShared<s32>(m_contexts.size() + 1);
}
} // namespace LittleEngine