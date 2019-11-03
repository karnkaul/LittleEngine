#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/LayerID.h"
#include "UIObject.h"
#include "UITransform.h"
#include "UIText.h"

namespace LittleEngine
{
// \brief Base UI WorldEntity class: uses UITransform, is capable of drawing a panel/an image/some text
class UIElement : public UIObject
{
private:
	struct UIPanel
	{
		Fixed border;
		Colour fill;
		Colour outline;
	};

public:
	UITransform m_transform;

protected:
	class SFRect* m_pRect = nullptr;
	class SFText* m_pText = nullptr;
	bool m_bLerpSize = false;

private:
	UIText m_uiText;
	UIPanel m_uiPanel;
	LayerID m_layer;
	class World* m_pWorld;
	class FontAsset* m_pFont = nullptr;
	bool m_bPanel = false;
	bool m_bDoTick = true;
	bool m_bStopTicking = false;
	bool m_bTicked = false;

public:
	UIElement(LayerID layer = LayerID::UI, bool bSilent = false);
	~UIElement() override;

	void SetParent(UITransform& parent);
	void SetPanel(UByte r = 255, UByte g = 255, UByte b = 255, UByte a = 128);
	void SetPanel(Colour fill, Fixed border = Fixed::Zero, Colour outline = Colour::Transparent);
	// void SetImage(class TextureAsset& texture, Colour colour = Colour::White);
	void SetText(UIText uiText);
	void SetTextColour(Colour colour);
	void SetFont(class FontAsset& font);
	void SetRectSize(Vector2 size, bool bAutoPad = false);
	void SetRectMaxSize();
	void SetAutoPad(bool bAutoPad);

	Vector2 RectSize() const;
	SFRect* Rect() const;
	SFText* Text() const;

	void OnCreate(LEContext& context, std::string name, UITransform* pParent = nullptr, FontAsset* pfont = nullptr);
	void SetEnabled(bool bEnabled);
	void Tick(Time dt = Time::Zero) override;

	LayerID GetLayer() const;
	void Regenerate(LayerID newLayer);

	void SetStatic(bool bStatic);

protected:
	virtual void OnCreated();

private:
	void Construct();
	void ApplyText();
	void ApplyPanel();

	friend class UIManager;
	friend class UIWidget;
	friend class UIContext;
};
} // namespace LittleEngine
