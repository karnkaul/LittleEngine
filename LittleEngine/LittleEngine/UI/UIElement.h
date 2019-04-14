#pragma once
#include "CoreTypes.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/SFLayerID.h"
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
	LayerID m_layer = LAYER_UI;

protected:
	class SFPrimitive* m_pPrimitive;
	SFPrimitive* m_pText;

private:
	class World* m_pWorld;
	bool m_bPanel = false;

public:
	UIElement(bool bSilent = false);
	UIElement(String name, bool bSilent = false);
	~UIElement() override;

	void SetParent(UITransform& parent);
	void SetPanel(UByte r = 255, UByte g = 255, UByte b = 255, UByte a = 128);
	void SetPanel(Colour fill, Fixed border = Fixed::Zero, Colour outline = Colour::Transparent);
	void SetImage(class TextureAsset& texture, Colour colour = Colour::White);
	void SetText(UIText uiText);
	void SetFont(class FontAsset& font);

	SFPrimitive* GetPrimitive() const;
	SFPrimitive* GetText() const;

	void OnCreate(String name, UITransform* pParent = nullptr);
	void Tick(Time dt) override;

protected:
	virtual void OnCreated();

private:
	void Construct();

	friend class UIManager;
	friend class UIWidget;
	friend class UIContext;
};
} // namespace LittleEngine
