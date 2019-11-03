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
		using GameInputs = std::vector<KeyType>;

		std::vector<KeyType> pressed;
		std::vector<KeyType> held;
		std::vector<KeyType> released;
		TextInput textInput;
		MouseInput mouseInput;
		JoyInput joyInput;

		static std::string Clipboard();

		bool IsPressed(s32 key) const;
		bool IsHeld(s32 key) const;
		bool IsReleased(s32 key) const;

		bool IsPressed(std::initializer_list<s32> keys, bool bAny = true) const;
		bool IsHeld(std::initializer_list<s32> keys, bool bAny = true) const;
		bool IsReleased(std::initializer_list<s32> keys, bool bAny = true) const;

		bool HasData() const;

		Fixed MouseWhellScroll() const;

	private:
		inline bool Result(std::initializer_list<s32> keys, std::function<bool(s32)> subroutine, bool bAny) const
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

private:
	struct InputContext
	{
		Delegate callback;
		WToken wToken;
		bool bForce = false;
	};

private:
	std::vector<KeyType> m_previousSnapshot;
	std::vector<KeyType> m_currentSnapshot;
	TextInput m_textInput;
	MouseInput m_mouseInput;
	JoyInput m_joyInput;
	std::vector<InputContext> m_contexts;
	std::optional<InputContext> m_oSudoContext;
	mutable s32 m_nextToken = 0;
#if defined(DEBUGGING)
	class Quad* m_pMouseH = nullptr;
	Quad* m_pMouseV = nullptr;
#endif

public:
	LEInput(InputMap inputMap);
	~LEInput();

public:
	Token Register(Delegate callback, bool bForce = false);
	MouseInput MouseState() const;

private:
	Token RegisterSudo(Delegate callback);
	void TakeSnapshot();
	void FireCallbacks();
#if defined(DEBUGGING)
	void CreateDebugPointer(class LERenderer& renderer);
#endif

	Token CreateToken() const;

	friend class Debug::ConsoleInput;
	friend class LEContext;
};
} // namespace LittleEngine
