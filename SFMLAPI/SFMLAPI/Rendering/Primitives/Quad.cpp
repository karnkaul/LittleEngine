#include "stdafx.h"
#include "SFML/Graphics/Vertex.hpp"
#include "Quad.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
Quad::Quad(LayerID layer) : APrimitive(layer), m_sfVertArr(sf::VertexArray(sf::Quads, 4))
{
}

Quad::~Quad() = default;

void Quad::OnUpdateRenderState(Fixed alpha)
{
	Matrix3 transform = GetMVP(alpha);
	Colour colour = Colour::Lerp(m_renderState.sfPrimaryColour, alpha);
	size_t idx = 0;
	for (auto& vertex : m_vertices)
	{
		Vector2 position = vertex.xy * transform;
		Vector2 size = m_quadRenderState.pTexture ? m_quadRenderState.pTexture->GetTextureSize() : Vector2::Zero;
		Vector2 uv = {size.x * vertex.uv.x, size.y * vertex.uv.y};
		m_sfVertArr[idx++] = sf::Vertex(Cast(position), Cast(colour), Cast(uv));
	}
}

void Quad::OnSwapState()
{
	m_quadRenderState = m_quadGameState;
}

void Quad::OnDraw(SFViewport& viewport, sf::RenderStates& sfStates)
{
	if (m_quadRenderState.pTexture)
	{
		sfStates.texture = &m_quadRenderState.pTexture->m_sfTexture;
	}
	viewport.draw(m_sfVertArr, sfStates);
}

Rect2 Quad::GetBounds() const
{
	Vector2 size(Maths::Abs(m_vertices[0].xy.x - m_vertices[1].xy.x),
				 Maths::Abs(m_vertices[1].xy.y - m_vertices[2].xy.y));
	return Rect2::CentreSize(size);
}

Quad* Quad::SetModel(Rect2 xy, Rect2 uv, Colour colour)
{
	m_vertices[0] = {WorldToScreen(xy.GetTopLeft()), uv.GetTopLeft(), colour};
	m_vertices[1] = {WorldToScreen(xy.GetTopRight()), uv.GetTopRight(), colour};
	m_vertices[2] = {WorldToScreen(xy.GetBottomRight()), uv.GetBottomRight(), colour};
	m_vertices[3] = {WorldToScreen(xy.GetBottomLeft()), uv.GetBottomLeft(), colour};
	m_gameState.sfPosition = WorldToScreen(xy.GetCentre());
	return this;
}

Quad* Quad::SetTexture(TextureAsset& texture)
{
	m_quadGameState.pTexture = &texture;
	return this;
}

Quad* Quad::SetUV(Rect2 uv)
{
	m_vertices[0].uv = uv.GetTopLeft();
	m_vertices[1].uv = uv.GetTopRight();
	m_vertices[2].uv = uv.GetBottomRight();
	m_vertices[3].uv = uv.GetBottomLeft();
	return this;
}

Quad* Quad::SetUV(Fixed u, Fixed v, Fixed du, Fixed dv)
{
	return SetUV(Rect2::TLSize({du - u, dv - v}, {u, v}));
}

Matrix3 Quad::GetMVP(Fixed alpha) const
{
	Vector2 s = m_renderState.sfScale.Lerp(alpha);
	Vector2 o = Vector2::ToOrientation(m_renderState.sfOrientation.Lerp(alpha));
	Vector2 p = m_renderState.sfPosition.Lerp(alpha);
	return Matrix3(p, o, s);
}
} // namespace LittleEngine
