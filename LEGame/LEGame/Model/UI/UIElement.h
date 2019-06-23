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
public:
	UITransform m_transform;
	
protected:
	class SFRect* m_pRect;
	class SFText* m_pText;

private:
	LayerID m_layer;
	class World* m_pWorld;
	bool m_bPanel = false;

public:
	UIElement(LayerID layer = LAYER_UI, bool bSilent = false);
	~UIElement() override;

	void SetParent(UITransform& parent);
	void SetPanel(UByte r = 255, UByte g = 255, UByte b = 255, UByte a = 128);
	void SetPanel(Colour fill, Fixed border = Fixed::Zero, Colour outline = Colour::Transparent);
	//void SetImage(class TextureAsset& texture, Colour colour = Colour::White);
	void SetText(UIText uiText);
	void SetTextColour(Colour colour);
	void SetFont(class FontAsset& font);

	SFRect* GetRect() const;
	SFText* GetText() const;

	void OnCreate(LEContext& context, String name, UITransform* pParent = nullptr);
	void Tick(Time dt = Time::Zero) override;

	LayerID GetLayer() const;

protected:
	virtual void OnCreated();

private:
	void Construct();

	friend class UIManager;
	friend class UIWidget;
	friend class UIContext;
};
} // namespace LittleEngine
