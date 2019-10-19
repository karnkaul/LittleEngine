#pragma once
#include "UIButton.h"

namespace LittleEngine
{
class UISelection : public UIButton
{
public:
	using OnChanged = LE::Delegate<std::pair<size_t, std::string>>;

private:
	struct UISelectionData
	{
		Vector2 panelSize = Vector2(500, 600);
		Colour panelColour = Colour(100, 100, 100, 240);
	};

	UISelectionData m_data;
	OnChanged m_onChanged;
	class UIButtonDrawer* m_pDrawer = nullptr;
	Token m_buttonToken;
	std::vector<Token> m_drawerTokens;
	std::vector<std::string> m_options;
	std::string m_value;

public:
	Token RegisterOnChanged(OnChanged::Callback callback);
	UISelection* SetValue(std::string text);
	UISelection* AddOption(std::string option);
	UISelection* SetOptions(std::vector<std::string> options);
	UISelection* SetPanelSize(Vector2 size);
	UISelection* SetPanelColour(Colour colour);

	const std::string& CurrentValue() const;
	std::vector<std::string>& Options();

protected:
	void OnCreated() override;

private:
	void OnSpawnDrawer();
};
} // namespace LittleEngine
