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
	UISelection();
	UISelection(String name);
	~UISelection() override;

	OnChanged::Token RegisterOnChanged(const OnChanged::Callback& callback);
	UISelection* SetValue(const String& text);
	UISelection* AddOption(const String& option);
	UISelection* AddOptions(const Vec<String>& options);
	UISelection* AddOptions(Vec<String>&& options);
	UISelection* SetPanelSize(Vector2 size);
	UISelection* SetPanelColour(Colour colour);

	const String& GetCurrentValue() const;
	Vec<String>& GetOptions();

protected:
	void OnInitWidget() override;

private:
	void OnSpawnDrawer();
};
} // namespace LittleEngine