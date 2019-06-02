#include "stdafx.h"
#include "SFQuad.h"
#include "SFMLAPI/System/SFTypes.h"
#include "SFMLAPI/System/SFAssets.h"

namespace LittleEngine
{
SFVertex::SFVertex() : colour(Colour::White)
{
}

SFVertex::SFVertex(Vector2 position, SFTexCoords texCoords, Colour colour)
	: position(position), texCoords(texCoords), colour(colour)
{
}

sf::Vertex SFVertex::ToSFVertex() const
{
	return sf::Vertex(Cast(WorldToScreen(position)), Cast(colour), texCoords.ToSFV2f());
}

SFQuad::SFQuad(Rect2 worldRect, SFTexRect texRect, Colour colour)
	: m_texRect(std::move(texRect)), m_orgSize(worldRect.GetSize()), m_position(worldRect.GetCentre())
{
	m_gameState.vertices[0] =
		SFVertex(worldRect.GetTopLeft(), SFTexCoords(m_texRect.min.x, m_texRect.min.y), colour);
	m_gameState.vertices[1] =
		SFVertex(worldRect.GetTopRight(), SFTexCoords(m_texRect.max.x, m_texRect.min.y), colour);
	m_gameState.vertices[2] =
		SFVertex(worldRect.GetBottomRight(), SFTexCoords(m_texRect.max.x, m_texRect.max.y), colour);
	m_gameState.vertices[3] =
		SFVertex(worldRect.GetBottomLeft(), SFTexCoords(m_texRect.min.x, m_texRect.max.y), colour);
	m_renderState = m_gameState;
}

SFQuad::~SFQuad() = default;

SFQuad* SFQuad::SetPosition(Vector2 position)
{
	Vector2 delta = position - m_position;
	for (auto& vertex : m_gameState.vertices)
	{
		vertex.position += delta;
	}
	m_position += delta;
	return this;
}

SFQuad* SFQuad::SetScale(Vector2 scale)
{
	m_scale = scale;
	Vector2 newScale = Vector2(m_orgSize.x * m_scale.x, m_orgSize.y * m_scale.y);
	Rect2 newRect = Rect2::CentreSize(newScale, m_position);
	m_gameState.vertices[0].position = newRect.GetTopLeft();
	m_gameState.vertices[1].position = newRect.GetTopRight();
	m_gameState.vertices[2].position = newRect.GetBottomRight();
	m_gameState.vertices[3].position = newRect.GetBottomLeft();
	return this;
}

SFQuad* SFQuad::SetWorldOrientation(Fixed degrees)
{
	Fixed rad = degrees * Maths::DEG_TO_RAD;
	Fixed s = rad.Sin(), c = rad.Cos();
	Vector2 size(m_orgSize.x * m_scale.x, m_orgSize.y * m_scale.y);
	Rect2 newRect = Rect2::CentreSize(size);
	Vector2 tl = newRect.GetTopLeft();
	Vector2 tr = newRect.GetTopRight();
	Vector2 bl = newRect.GetBottomLeft();
	Vector2 br = newRect.GetBottomRight();
	m_gameState.vertices[0].position = Vector2((tl.x * c) - (tl.y * s), (tl.x * s) + (tl.y * c));
	m_gameState.vertices[1].position = Vector2((tr.x * c) - (tr.y * s), (tr.x * s) + (tr.y * c));
	m_gameState.vertices[2].position = Vector2((br.x * c) - (br.y * s), (br.x * s) + (br.y * c));
	m_gameState.vertices[3].position = Vector2((bl.x * c) - (bl.y * s), (bl.x * s) + (bl.y * c));
	return this;
}

SFQuad* SFQuad::SetLocalOrientation(Fixed orientation)
{
	Vector2 position = m_position;
	SetPosition(Vector2::Zero);
	SetWorldOrientation(orientation);
	SetPosition(position);
	return this;
}

SFQuad* SFQuad::SetColour(Colour colour)
{
	for (auto& vertex : m_gameState.vertices)
	{
		vertex.colour = colour;
	}
	return this;
}

SFQuad* SFQuad::SetTexRect(SFTexRect texRect)
{
	m_texRect = std::move(texRect);
	m_gameState.vertices[0].texCoords = {m_texRect.min.x, m_texRect.min.y};
	m_gameState.vertices[1].texCoords = {m_texRect.max.x, m_texRect.min.y};
	m_gameState.vertices[2].texCoords = {m_texRect.max.x, m_texRect.max.y};
	m_gameState.vertices[3].texCoords = {m_texRect.min.x, m_texRect.max.y};
	return this;
}

SFQuad* SFQuad::SetEnabled(bool bEnabled)
{
	m_gameState.bEnabled = bEnabled;
	return this;
}


void SFQuad::SwapStates()
{
	m_renderState = m_gameState;
}

SFQuadVec::SFQuadVec()
{
	m_quads.reserve(1024);
}

SFQuad* SFQuadVec::AddQuad()
{
	if (m_pTexture)
	{
		Vector2 texSize = m_pTexture->GetTextureSize();
		SFTexRect texRect(texSize.x.ToS32(), texSize.y.ToS32());
		UPtr<SFQuad> uQuad = MakeUnique<SFQuad>(Rect2::CentreSize(texSize), texRect);
		SFQuad* pQuad = uQuad.get();
		m_quads.emplace_back(std::move(uQuad));
		return pQuad;
	}
	return nullptr;
}

void SFQuadVec::SetTexture(TextureAsset& texture)
{
	m_pTexture = &texture;
}


void SFQuadVec::SwapStates()
{
	for (auto& uQuad : m_quads)
	{
		uQuad->SwapStates();
	}
}

bool SFQuadVec::IsPopulated() const
{
	return !m_quads.empty();
}

sf::VertexArray SFQuadVec::ToSFVertexArray() const
{
	Vec<SFQuad*> enabledQuads;
	enabledQuads.reserve(m_quads.size());
	for (auto& uQuad : m_quads)
	{
		if (uQuad->m_renderState.bEnabled)
		{
			enabledQuads.push_back(uQuad.get());
		}
	}
	sf::VertexArray va(sf::Quads, enabledQuads.size() * 4);
	size_t idx = 0;
	for (auto pQuad : enabledQuads)
	{
		Assert(pQuad->m_renderState.vertices.size() == 4, "Invalid vertex count on SFQuad!");
		for (auto& vertex : pQuad->m_renderState.vertices)
		{
			va[idx++] = vertex.ToSFVertex();
		}
	}
	return va;
}

sf::RenderStates SFQuadVec::ToSFRenderStates() const
{
	sf::RenderStates states;
	if (m_pTexture)
	{
		states.texture = &m_pTexture->m_sfTexture;
	}
	return states;
}
} // namespace LittleEngine