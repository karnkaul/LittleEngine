#include "stdafx.h"
#include "Core/Matrix3.h"
#include "Core/Logger.h"
#include "SFQuad.h"
#include "SFMLAPI/System/SFTypes.h"
#include "SFMLAPI/System/SFAssets.h"

namespace LittleEngine
{
using Matrix3 = Core::Matrix3;

SFQuad::SFQuad(Rect2 worldRect, SFTexRect texRect, Colour colour)
	: m_texRect(std::move(texRect)), m_size(worldRect.GetSize())
{
	m_vertices[0] = {worldRect.GetTopLeft(), SFTexCoords(m_texRect.min.x, m_texRect.min.y), colour};
	m_vertices[1] = {worldRect.GetTopRight(), SFTexCoords(m_texRect.max.x, m_texRect.min.y), colour};
	m_vertices[2] = {worldRect.GetBottomRight(), SFTexCoords(m_texRect.max.x, m_texRect.max.y), colour};
	m_vertices[3] = {worldRect.GetBottomLeft(), SFTexCoords(m_texRect.min.x, m_texRect.max.y), colour};
	m_gameState.position = worldRect.GetCentre();
}

SFQuad::~SFQuad() = default;

SFQuad* SFQuad::SetPosition(Vector2 position, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.position.Reset(position);
	}
	else
	{
		m_gameState.position.Update(position);
	}
	return this;
}

SFQuad* SFQuad::SetScale(Vector2 scale, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.scale.Reset(scale);
	}
	else
	{
		m_gameState.scale.Update(scale);
	}
	return this;
}

SFQuad* SFQuad::SetOrientation(Fixed degrees, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.orientation.Reset(degrees);
	}
	else
	{
		m_gameState.orientation.Update(degrees);
	}
	return this;
}

SFQuad* SFQuad::SetColour(Colour colour, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.colour.Reset(colour);
	}
	else
	{
		m_gameState.colour.Update(colour);
	}
	return this;
}

SFQuad* SFQuad::SetTexRect(SFTexRect texRect)
{
	m_texRect = std::move(texRect);
	m_vertices[0].uv = {m_texRect.min.x, m_texRect.min.y};
	m_vertices[1].uv = {m_texRect.max.x, m_texRect.min.y};
	m_vertices[2].uv = {m_texRect.max.x, m_texRect.max.y};
	m_vertices[3].uv = {m_texRect.min.x, m_texRect.max.y};
	return this;
}

SFQuad* SFQuad::SetUV(Fixed u, Fixed v, Fixed du, Fixed dv)
{
	Vector2 min(u * m_size.x, v * m_size.y);
	Vector2 max(du * m_size.x, dv * m_size.y);
	return SetTexRect(SFTexRect(SFTexCoords(min), SFTexCoords(max)));
}

SFQuad* SFQuad::SetEnabled(bool bEnabled)
{
	m_gameState.bEnabled = bEnabled;
	return this;
}

void SFQuad::SwapState()
{
	m_renderState = m_gameState;
}

void SFQuad::Reconcile()
{
	m_gameState.position.min = m_gameState.position.max;
	m_gameState.scale.min = m_gameState.scale.max;
	m_gameState.orientation.min = m_gameState.orientation.max;
	m_gameState.colour.min = m_gameState.colour.max;
}

SFQuadVec::SFQuadVec()
{
	m_quads.reserve(1024);
}

SFQuad* SFQuadVec::AddQuad()
{
	Assert(m_pTexture, "Texture is null!");
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

void SFQuadVec::SwapState()
{
	for (auto& uQuad : m_quads)
	{
		uQuad->SwapState();
	}
}

void SFQuadVec::Reconcile()
{
	for (auto& uQuad : m_quads)
	{
		uQuad->Reconcile();
	}
}

bool SFQuadVec::IsPopulated() const
{
	return !m_quads.empty();
}

sf::VertexArray SFQuadVec::ToSFVertexArray(Fixed alpha) const
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
		Vector2 s = pQuad->m_renderState.scale.Lerp(alpha);
		Vector2 o = Vector2::ToOrientation(pQuad->m_renderState.orientation.Lerp(alpha));
		Vector2 p = pQuad->m_renderState.position.Lerp(alpha);
		Colour colour = Colour::Lerp(pQuad->m_renderState.colour, alpha);
		Matrix3 transform(p, o, s);
		for (auto& vertex : pQuad->m_vertices)
		{
			Vector2 position = WorldToScreen(vertex.xy * transform);
			sf::Vector2f uv = vertex.uv.ToSFV2f();
			va[idx++] = sf::Vertex(Cast(position), Cast(colour), uv);
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