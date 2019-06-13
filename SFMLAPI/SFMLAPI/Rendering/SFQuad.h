#pragma once
#include "Core/CoreTypes.h"
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFRenderState.h"
#include "SFTexRect.h"

namespace LittleEngine
{
struct SFVertex
{
	Vector2 xy;
	SFTexCoords uv;
	Colour colour;
};

struct SFQuadState
{
	TRange<Vector2> position = Vector2::Zero;
	TRange<Vector2> scale = Vector2::One;
	TRange<Fixed> orientation = Fixed::Zero;
	TRange<Colour> colour = Colour::White;
	
	bool bEnabled = false;
};

class SFQuad
{
#if DEBUGGING
public:
	bool m_bDebugThisQuad = false;
#endif
private:
	SFQuadState m_gameState;
	SFQuadState m_renderState;
	Array<SFVertex, 4> m_vertices;
	SFTexRect m_texRect;
	Vector2 m_size;
	
public:
	SFQuad(Rect2 worldRect, SFTexRect texRect = SFTexRect::Zero, Colour colour = Colour::White);
	virtual ~SFQuad();

	SFQuad* SetPosition(Vector2 position, bool bImmediate = false);
	SFQuad* SetScale(Vector2 scale, bool bImmediate = false);
	SFQuad* SetOrientation(Fixed degrees, bool bImmediate = false);
	SFQuad* SetColour(Colour colour, bool bImmediate = false);
	SFQuad* SetTexRect(SFTexRect texRect);
	SFQuad* SetUV(Fixed u, Fixed v, Fixed du, Fixed dv);
	SFQuad* SetEnabled(bool bEnabled);

private:
	void SwapState();
	void Reconcile();

	friend class SFQuadVec;
};

class SFQuadVec
{
public:
	Vec<UPtr<SFQuad>> m_quads;
	class TextureAsset* m_pTexture = nullptr;

public:
	SFQuadVec();

	// Sets size and texCoords according to m_pTexture
	SFQuad* AddQuad();
	void SetTexture(TextureAsset& texture);

	void SwapState();
	void Reconcile();

	bool IsPopulated() const;

private:
	sf::VertexArray ToSFVertexArray(Fixed alpha) const;
	sf::RenderStates ToSFRenderStates() const;

	friend class SFRenderer;
};
} // namespace LittleEngine
