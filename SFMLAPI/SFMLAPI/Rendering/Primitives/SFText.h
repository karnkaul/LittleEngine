#pragma once
#include "SFML/Graphics/Text.hpp"
#include "SFDrawable.h"

namespace LittleEngine
{
class SFText : public ASFDrawable
{
protected:
	struct TextState
	{
		String text;
		u32 size = 20;
	};

protected:
	TextState m_textGameState;
	TextState m_textRenderState;
	sf::Text m_sfText;
	class FontAsset* m_pFont = nullptr;
	std::atomic<bool> m_bTextChanged = false;

public:
	SFText(LayerID layer);
	~SFText() override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(SFViewport& viewport, sf::RenderStates& states) override;
	void OnSwapState() override;

public:
	Rect2 GetBounds() const override;

public:
	SFText* SetFont(FontAsset& font);
	SFText* SetSize(u32 size);
	SFText* SetText(String text);
};
}