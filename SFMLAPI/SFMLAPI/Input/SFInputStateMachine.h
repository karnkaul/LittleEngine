#pragma once
#include "SFML/Window/Event.hpp"
#include "Core/CoreTypes.h"
#include "SFInputDataFrame.h"

namespace LittleEngine
{
// \brief Concrete class that a Graphics can update KeyStates to every frame
class SFInputStateMachine
{
private:
	TextInput m_textInput;
	MouseInput m_pointerInput;
	JoyInput m_joyInput;
	InputMap m_inputMap;
	Vec<KeyState> m_keyStates;

public:
	SFInputStateMachine();
	~SFInputStateMachine();

	// Call this to check if a Key was pressed in this frame
	bool IsKeyPressed(KeyType key) const;
	// Get KeyState for this KeyCode on the current frame
	const KeyState* GetKeyState(KeyType key) const;
	
	void SetInputMapping(InputMap inputMap);
	SFInputDataFrame GetFrameInputData() const;

private:
	KeyState& GetOrCreateKeyState(KeyType key);

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

	friend class SFInputHandler;
};
} // namespace LittleEngine
