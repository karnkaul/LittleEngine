#pragma once
#include <functional>
#include "Gamepad.h"
#if DEBUGGING
#include "SFMLAPI/Rendering/Colour.h"
#endif

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
	MouseInput m_mouseInput;
	Vec<InputContext> m_contexts;
	UPtr<InputContext> m_uSudoContext;
	Gamepad m_gamepad;
#if DEBUGGING
	class SFPrimitive* m_pMouseH = nullptr;
	SFPrimitive* m_pMouseV = nullptr;
#endif

public:
	EngineInput();

public:
	Token Register(Delegate callback);
	MouseInput GetMouseState() const;

private:
	Token RegisterSudo(Delegate callback);
	void TakeSnapshot(const struct SFInputDataFrame& frameData);
	void FireCallbacks();
	void BindDefaults();
#if DEBUGGING
	void CreateDebugPointer();
#endif

	Token CreateToken() const;

	friend class Debug::ConsoleInput;
	friend class EngineLoop;
	friend class EngineService;
};
} // namespace LittleEngine
