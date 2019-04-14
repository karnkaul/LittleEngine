#pragma once
#include "CoreTypes.h"
#include "Colour.h"
#include "SFRenderState.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
enum class SFShapeType
{
	Rectangle,
	Circle,
};

// Origin at top-left of texture; +X right, +Y down
struct SFTexCoords
{
	u32 x = 0;
	u32 y = 0;

	SFTexCoords() = default;
	//+X right, +Y down
	SFTexCoords(u32 x, u32 y);
};

// Origin at top-left of texture; +X right, +Y down
struct SFTexRect
{
	SFTexCoords min;
	SFTexCoords max;

	SFTexRect() = default;
	//+X right, +Y down
	SFTexRect(SFTexCoords min, SFTexCoords max);

	sf::IntRect Cast() const;
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
	static Vector2 WorldToScreen(Vector2 worldPoint);
	static Fixed WorldToScreen(Fixed worldOrientation);
	static Vector2 ScreenToWorld(Vector2 screenPoint);
	static Fixed ScreenToWorld(Fixed screenOrientation);

	SFPrimitive();
	
public:
	// Interpolated States
	SFPrimitive* SetEnabled(bool bEnabled);
	SFPrimitive* SetLayer(LayerID layer);
	SFPrimitive* SetPosition(Vector2 sfPosition, bool bImmediate = false);
	SFPrimitive* SetOrientation(Fixed sfOrientation, bool bImmediate = false);
	SFPrimitive* SetScale(Vector2 sfScale, bool bImmediate = false);
	SFPrimitive* SetPivot(Vector2 pivot = Vector2::Zero);
	SFPrimitive* SetPrimaryColour(Colour sfColour, bool bImmediate = false);
	SFPrimitive* SetSecondaryColour(Colour sfColour, bool bImmediate = false);

	// Shapes and Text
	SFPrimitive* SetOutline(Fixed thickness);

	// Shapes
	SFPrimitive* SetSize(Vector2 size, SFShapeType onShape);

	// Sprite
	SFPrimitive* SetTexture(const class TextureAsset & texture);
	SFPrimitive* CropTexture(const Rect2& rect);
	SFPrimitive* CropTexture(SFTexRect textureRect);

	// Text
	SFPrimitive* SetFont(const class FontAsset& font);
	SFPrimitive* SetTextSize(u32 pixelSize);
	SFPrimitive* SetText(String text);

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
	void UpdateRenderState(Fixed alpha);

	friend struct GFXDataFrame;
	friend class SFRenderer;
};
} // namespace LittleEngine
