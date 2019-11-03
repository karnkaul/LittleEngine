#pragma once
#include "SFML/Graphics/Text.hpp"
#include "SFDrawable.h"

namespace LittleEngine
{
class SFText : public ASFDrawable
{
protected:
	// SFText does not maintain separate game and render states,
	// but instead uses a mutex lock when setting text / drawing.
	// This is because it is otherwise impossible to obtain size (bounds) immediately.
	sf::Text m_sfText;
	std::string m_text;
	u32 m_textSize = 20;
	std::mutex m_textMutex;
	class FontAsset* m_pFont = nullptr;

public:
	SFText(LayerID layer);
	~SFText() override;

public:
	Rect2 GameBounds() const override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(Viewport& viewport, sf::RenderStates& sfStates) override;
	Vector2 SFSize() const override;
	sf::FloatRect SFBounds() const override;

public:
	SFText* SetFont(FontAsset& font);
	SFText* SetSize(u32 size);
	SFText* SetText(std::string text);

	std::string Text() const;
	u32 TextSize() const;
	bool IsFontSet() const;

private:
	void UpdateSFText();
};
} // namespace LittleEngine
