#pragma once
#include "Core/CoreTypes.h"
#include "SFML/Graphics/Vertex.hpp"
#include "SFRenderState.h"

namespace LittleEngine
{
struct SFVertex
{
	Vector2 position;
	SFTexCoords texCoords;
	Colour colour;

	SFVertex();
	SFVertex(Vector2 position, SFTexCoords texCoords = SFTexCoords::Zero, Colour colour = Colour::White);

	sf::Vertex ToSFVertex() const;
};

struct SFQuadState
{
	Array<SFVertex, 4> vertices;
	bool bEnabled = false;
};

class SFQuad
{
private:
	SFQuadState m_gameState;
	SFQuadState m_renderState;
	SFTexRect m_texRect;
	Vector2 m_orgSize;
	Vector2 m_scale = Vector2::One;
	Vector2 m_position;
	
public:
	SFQuad(Rect2 worldRect, SFTexRect texRect = SFTexRect::Zero, Colour colour = Colour::White);
	~SFQuad();

	SFQuad* SetPosition(Vector2 position);
	// Warning: Resets orientation
	SFQuad* SetScale(Vector2 scale);
	SFQuad* SetWorldOrientation(Fixed orientation);
	SFQuad* SetLocalOrientation(Fixed orientation);
	SFQuad* SetColour(Colour colour);
	SFQuad* SetTexRect(SFTexRect texRect);
	SFQuad* SetEnabled(bool bEnabled);

private:
	void SwapStates();

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

	void SwapStates();

	bool IsPopulated() const;
	sf::VertexArray ToSFVertexArray() const;
	sf::RenderStates ToSFRenderStates() const;
};
} // namespace LittleEngine