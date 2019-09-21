#pragma once
#include <optional>
#include "SFMLAPI/Input/InputHandler.h"
#include "SFMLAPI/Input/InputDataFrame.h"
#if defined(DEBUGGING)
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/LayerID.h"
#endif

namespace LittleEngine
{
using namespace std::placeholders;
namespace Debug
{
class ConsoleInput;
}

class LEInput final : public InputHandler
{
public:
	struct Frame
	{
		using GameInputs = Vec<KeyType>;

		Vec<KeyType> pressed;
		Vec<KeyType> held;
		Vec<KeyType> released;
		TextInput textInput;
		MouseInput mouseInput;
		JoyInput joyInput;

		static String Clipboard();

		bool IsPressed(s32 key) const;
		bool IsHeld(s32 key) const;
		bool IsReleased(s32 key) const;

		bool IsPressed(InitList<s32> keys, bool bAny = true) const;
		bool IsHeld(InitList<s32> keys, bool bAny = true) const;
		bool IsReleased(InitList<s32> keys, bool bAny = true) const;

		bool HasData() const;

		Fixed MouseWhellScroll() const;

	private:
		inline bool Result(InitList<s32> keys, std::function<bool(s32)> subroutine, bool bAny) const
		{
			bool bResult = bAny ? false : true;
			for (auto key : keys)
			{
				if (bAny)
				{
					bResult |= subroutine(key);
				}
				else
				{
					bResult &= subroutine(key);
				}
			}
			return bResult;
		}
	};

	using Delegate = std::function<bool(const Frame& frameData)>;
	using Token = SPtr<s32>;

private:
	using WToken = WPtr<s32>;

	struct InputContext
	{
		Delegate callback;
		WToken wToken;
		bool bForce = false;
	};

private:
	Vec<KeyType> m_previousSnapshot;
	Vec<KeyType> m_currentSnapshot;
	TextInput m_textInput;
	MouseInput m_mouseInput;
	JoyInput m_joyInput;
	Vec<InputContext> m_contexts;
	std::optional<InputContext> m_oSudoContext;
	class LEContext* m_pContext;
#if defined(DEBUGGING)
	LayerID mouseLayer = LayerID::TopFull;
	class Quad* m_pMouseH = nullptr;
	Quad* m_pMouseV = nullptr;
#endif

public:
	LEInput(LEContext& context, InputMap inputMap);
	~LEInput();

public:
	Token Register(Delegate callback, bool bForce = false);
	MouseInput MouseState() const;

private:
	Token RegisterSudo(Delegate callback);
	void TakeSnapshot();
	void FireCallbacks();
#if defined(DEBUGGING)
	void CreateDebugPointer();
#endif

	Token CreateToken() const;

	friend class Debug::ConsoleInput;
	friend class LEContext;
};
} // namespace LittleEngine
