#pragma once
#include "CoreTypes.h"
#include "Colour.h"
#include "SFRenderState.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
template <typename T>
using TRange = Core::TRange<T>;
using SFPosition = TRange<Vector2>;
using SFOrientation = TRange<Fixed>;
using SFScale = TRange<Vector2>;
using SFColour = TRange<Colour>;

enum class SFShapeType
{
	Rectangle,
	Circle,
};

struct SFPrimitiveData
{
	String text;
	Colour primary;
	Colour secondary;
	class TextureAsset* texture = nullptr;
	class FontAsset* font = nullptr;
};

class SFPrimitive final
{
#if DEBUGGING
public:
	bool bDebugThisPrimitive = false;
#endif
private:
	static constexpr u32 DEFAULT_TEXT_SIZE = 25;

	SFRenderState m_state;
	sf::CircleShape m_circle;
	sf::RectangleShape m_rectangle;
	sf::Sprite m_sprite;
	sf::Text m_text;
	bool m_bStatic = false;
	bool m_bMakeStatic = false;

public:
	static Vector2 WorldToScreen(const Vector2& worldPoint);
	static Fixed WorldToScreen(const Fixed& worldOrientation);
	static Vector2 ScreenToWorld(const Vector2& screenPoint);
	static Fixed ScreenToWorld(const Fixed& screenOrientation);

	SFPrimitive();
	SFPrimitive(const SFPrimitiveData& data);

public:
	// Interpolated States
	SFPrimitive* SetEnabled(bool bEnabled);
	SFPrimitive* SetLayer(LayerID layer);
	SFPrimitive* SetPosition(const Vector2& sfPosition, bool bImmediate = false);
	SFPrimitive* SetOrientation(const Fixed& sfOrientation, bool bImmediate = false);
	SFPrimitive* SetScale(const Vector2& sfScale, bool bImmediate = false);
	SFPrimitive* SetPivot(const Vector2& pivot = Vector2::Zero);
	SFPrimitive* SetPrimaryColour(Colour sfColour, bool bImmediate = false);
	SFPrimitive* SetSecondaryColour(Colour sfColour, bool bImmediate = false);

	// Shapes and Text
	SFPrimitive* SetOutline(const Fixed& thickness);

	// Shapes
	SFPrimitive* SetSize(const Vector2& size, SFShapeType onShape);

	// Sprite
	SFPrimitive* SetTexture(const class TextureAsset & texture);
	SFPrimitive* Crop(const Rect2& rect);

	// Text
	SFPrimitive* SetFont(const class FontAsset& font);
	SFPrimitive* SetTextSize(u32 pixelSize);
	SFPrimitive* SetText(const String& text);

	Rect2 GetBounds() const;
	Rect2 GetShapeBounds() const;
	Rect2 GetSpriteBounds() const;
	Rect2 GetTextBounds() const;

	bool IsEnabled() const;
	bool IsStatic() const;
	Vector2 GetPosition() const;
	Fixed GetOrientation() const;
	Vector2 GetScale() const;
	Colour GetPrimaryColour() const;
	Colour GetSecondaryColour() const;
	LayerID GetLayer() const;

	void ReconcileState();
	void SetStatic(bool bStatic);

private:
	void UpdatePivot();
	void UpdateRenderState(const Fixed& alpha);

	friend class GFXBuffer;
	friend struct GFXDataFrame;
	friend class SFRenderer;
};
} // namespace LittleEngine
