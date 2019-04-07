#pragma once
#include <functional>
#include "Gamepad.h"

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

		Frame(Vec<GameInputType> pressed,
			  Vec<GameInputType> held,
			  Vec<GameInputType> released,
			  TextInput textInput);

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
		Delegate callback;
		WToken wToken;

		InputContext(Delegate callback, Token& sToken);
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
	Token Register(const Delegate& Callback);

private:
	Token RegisterSudo(const Delegate& Callback);
	void TakeSnapshot(const struct SFInputDataFrame& frameData);
	void FireCallbacks();
	void BindDefaults();

	Token CreateToken() const;

	friend class Debug::ConsoleInput;
	friend class EngineLoop;
	friend class EngineService;
};
} // namespace LittleEngine
