#pragma once
#include "CoreTypes.h"
#include "SFInputDataFrame.h"

namespace LittleEngine
{
// \brief Wrapper struct that holds key state modifiers
struct KeyMod
{
	static const KeyMod Default;
	bool bControl = false;
	bool bAlt = false;
	bool bShift = false;

	KeyMod();
	KeyMod(bool bControl, bool bAlt, bool bShift);

private:
	KeyMod(const sf::Event::KeyEvent& event);
	friend class InputHandler;
};

// \brief Concrete class that a Graphics can update KeyStates to every frame
class SFInputStateMachine
{
private:
	TextInput m_textInput;
	Vec<KeyState> m_keyStates;

public:
	SFInputStateMachine();

	// Call this to check if a Key was pressed in this frame
	bool IsKeyPressed(KeyCode code) const;
	// Get KeyState for this KeyCode on the current frame
	const KeyState GetKeyState(KeyCode code) const;
	const SFInputDataFrame GetFrameInputData() const;

private:
	KeyState& GetOrCreateKeyState(KeyCode code);

	void OnKeyDown(const sf::Event::KeyEvent& key);
	void OnKeyUp(const sf::Event::KeyEvent& key);
	void ResetKeyStates();
	void ClearTextInput();
	void OnTextInput(u32 unicode);

	void StoreNonASCIISpecialInput(KeyCode key);

	friend class SFEventHandler;
};
} // namespace LittleEngine
