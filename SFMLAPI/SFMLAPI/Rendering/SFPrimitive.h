#pragma once
#include "Core/CoreTypes.h"
#include "Colour.h"
#include "SFQuad.h"
#include "SFRenderState.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
class SFPrimitive final
{
#if DEBUGGING
public:
	bool m_bDebugThisPrimitive = false;
#endif
public:
	bool m_bDestroyed = false;

private:
	static constexpr u32 DEFAULT_TEXT_SIZE = 25;

	SFRenderState m_gameState;
	SFRenderState m_renderState;
	SFQuadVec m_quadVec;
	sf::CircleShape m_circle;
	sf::RectangleShape m_rectangle;
	sf::Sprite m_sprite;
	sf::Text m_text;
	sf::Vector2f m_prevScale;
	class SFShader* m_pShader = nullptr;
	std::atomic<bool> m_bTextChanged = false;
	bool m_bWasDisabled = false;
	bool m_bStatic = false;
	bool m_bMakeStatic = false;
	bool m_bRendered = false;
	
public:
	SFPrimitive(LayerID layer);
	SFPrimitive(const SFPrimitive& toCopy) = delete;

	void SwapState();
	
public:
	// Interpolated States
	SFPrimitive* SetEnabled(bool bEnabled);
	SFPrimitive* SetPosition(Vector2 worldPosition, bool bImmediate = false);
	SFPrimitive* SetOrientation(Fixed worldOrientation, bool bImmediate = false);
	SFPrimitive* SetScale(Vector2 worldScale, bool bImmediate = false);
	SFPrimitive* SetPivot(Vector2 pivot = Vector2::Zero);
	SFPrimitive* SetPrimaryColour(Colour sfColour, bool bImmediate = false);
	SFPrimitive* SetSecondaryColour(Colour sfColour, bool bImmediate = false);

	// Shapes and Text
	SFPrimitive* SetOutline(Fixed thickness);

	// Shapes
	SFPrimitive* SetSize(Vector2 size, SFShapeType onShape);

	// Sprite
	SFPrimitive* SetTexture(const class TextureAsset & texture);
	SFPrimitive* CropTexture(SFTexRect textureRect);

	// Text
	SFPrimitive* SetFont(const class FontAsset& font);
	SFPrimitive* SetTextSize(u32 pixelSize);
	SFPrimitive* SetText(String text);

	SFPrimitive* SetStatic(bool bStatic);
	
	SFPrimitive* SetShader(SFShader* pShader);

	SFQuadVec* GetQuadVec();
	
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
	void Destroy();

private:
	void UpdatePivot();
	void UpdateRenderState(Fixed alpha);

	friend class SFRenderer;
	friend class SFShader;
};
} // namespace LittleEngine
