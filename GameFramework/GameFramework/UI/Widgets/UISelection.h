#pragma once
#include "UIButton.h"

namespace LittleEngine
{
class UISelection : public UIButton
{
public:
	using OnChanged = Core::Delegate<std::pair<size_t, String>>;

private:
	struct UISelectionData
	{
		Vector2 panelSize = Vector2(500, 600);
		Colour panelColour = Colour(100, 100, 100, 240);
	};

	UISelectionData m_data;
	OnChanged m_onChanged;
	class UIButtonDrawer* m_pDrawer = nullptr;
	SPtr<int> m_buttonToken;
	Vec<SPtr<int>> m_drawerTokens;
	Vec<String> m_options;
	String m_value;

public:
	OnChanged::Token RegisterOnChanged(OnChanged::Callback callback);
	UISelection* SetValue(String text);
	UISelection* AddOption(String option);
	UISelection* SetOptions(Vec<String> options);
	UISelection* SetPanelSize(Vector2 size);
	UISelection* SetPanelColour(Colour colour);

	const String& GetCurrentValue() const;
	Vec<String>& GetOptions();

protected:
	void OnCreated() override;

private:
	void OnSpawnDrawer();
};
} // namespace LittleEngine