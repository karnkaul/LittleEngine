#pragma once
#include "SFML/Window/Event.hpp"
#include "Core/CoreTypes.h"
#include "InputDataFrame.h"

namespace LittleEngine
{
// \brief Concrete class that a Graphics can update KeyStates to every frame
class InputStateMachine
{
private:
	TextInput m_textInput;
	MouseInput m_pointerInput;
	JoyInput m_joyInput;
	InputMap m_inputMap;
	UMap<s32, KeyState> m_keyStateMap;

public:
	InputStateMachine();
	~InputStateMachine();

	void SetInputMapping(InputMap inputMap);
	InputDataFrame Dataframe() const;

private:
	KeyState& GetOrCreateKeyState(KeyType key);
	void ModifyKeyState(KeyType key, bool bPressed);
	
	void OnKeyDown(const sf::Event::KeyEvent& key);
	void OnKeyUp(const sf::Event::KeyEvent& key);
	void OnMouseDown(const sf::Event::MouseButtonEvent& button);
	void OnMouseUp(const sf::Event::MouseButtonEvent& button);
	void OnMouseMove(Vector2 worldPosition);
	void OnMouseChange(bool bInViewport);
	void SetMouseWheelScroll(Fixed delta);

	void ResetKeyStates();
	void ClearTextInput();
	void OnTextInput(u32 unicode);
	void UpdateJoyInput();

	friend class InputHandler;
};
} // namespace LittleEngine
