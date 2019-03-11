#pragma once
#include <functional>
#include "Gamepad.h"
#include "SFMLAPI/Input/SFInputStateMachine.h"

namespace LittleEngine
{
using namespace std::placeholders;
namespace Debug
{
class ConsoleInput;
}

class EngineInput
{
public:
	struct Frame
	{
		using GameInputs = Vec<GameInputType>;

		Vec<GameInputType> pressed;
		Vec<GameInputType> held;
		Vec<GameInputType> released;
		TextInput textInput;
		
		static String GetClipboard();

		Frame(const Vec<GameInputType>& pressed,
			  const Vec<GameInputType>& held,
			  const Vec<GameInputType>& released,
			  const TextInput& special);

		bool IsPressed(GameInputType keyCode) const;
		bool IsHeld(GameInputType keyCode) const;
		bool IsReleased(GameInputType keyCode) const;
		bool HasData() const;
	};

	using Delegate = std::function<bool(const Frame& frameData)>;
	using Token = SPtr<s32>;

private:
	using WToken = WPtr<s32>;

	struct InputContext
	{
		Delegate Callback;
		WToken wToken;

		InputContext(Delegate Callback, Token& sToken);
	};

private:
	Vec<GameInputType> m_previousSnapshot;
	Vec<GameInputType> m_currentSnapshot;
	TextInput m_textInput;
	Vec<InputContext> m_contexts;
	UPtr<InputContext> m_uSudoContext;
	Gamepad m_gamepad;

public:
	EngineInput();

public:
	Token Register(Delegate Callback);

private:
	Token RegisterSudo(Delegate Callback);
	void TakeSnapshot(const SFInputDataFrame& frameData);
	void FireCallbacks();
	void BindDefaults();

	Token CreateToken() const;

	friend class Debug::ConsoleInput;
	friend class EngineLoop;
	friend class EngineService;
};
} // namespace LittleEngine
