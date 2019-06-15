#include "stdafx.h"
#include "Core/Logger.h"
#include "SFML/Graphics/Vertex.hpp"
#include "Quad.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/System/SFTypes.h"
#if ENABLED(RENDER_STATS)
#include "SFMLAPI/Rendering/SFRenderer.h"
#endif


namespace LittleEngine
{
#if ENABLED(RENDER_STATS)
extern RenderData g_renderData;
#endif

Quad::Quad(LayerID layer) : APrimitive(layer), m_sfVertArr(sf::VertexArray(sf::Quads, 4))
{
}

Quad::~Quad() = default;

void Quad::ReconcileGameState()
{
	APrimitive::ReconcileGameState();
	m_quadGameState.tUV.min = m_quadGameState.tUV.max;
}

void Quad::SwapState()
{
	APrimitive::SwapState();
	m_quadRenderState = m_quadGameState;
}

void Quad::OnUpdateRenderState(Fixed alpha)
{
	State state = GetState(alpha);
	Rect2 uvRect = Maths::Lerp(m_quadRenderState.tUV.min, m_quadRenderState.tUV.max, alpha);
	Vec<Vector2> uvs = {uvRect.GetTopLeft(), uvRect.GetTopRight(), uvRect.GetBottomRight(),
						uvRect.GetBottomLeft()};
	Matrix3 transform(state.position, Vector2::ToOrientation(state.orientation), state.scale);
	size_t idx = 0;
	for (auto& vertex : m_vertexModel)
	{
		Vector2 position = vertex * transform;
		Vector2 size = m_quadRenderState.pTexture ? m_quadRenderState.pTexture->GetTextureSize()
												  : Vector2::Zero;

		Vector2 uv = {size.x * uvs[idx].x, size.y * uvs[idx].y};
		m_sfVertArr[idx++] = sf::Vertex(Cast(position), Cast(state.colour), Cast(uv));
	}
}

void Quad::OnDraw(SFViewport& viewport, sf::RenderStates& sfStates)
{
#if ENABLED(RENDER_STATS)
	++g_renderData._quadCount_Internal;
#endif
	if (m_quadRenderState.pTexture)
	{
		sfStates.texture = &m_quadRenderState.pTexture->m_sfTexture;
	}
	viewport.draw(m_sfVertArr, sfStates);
}

Quad* Quad::SetModel(Rect2 xy)
{
	m_vertexModel = {WorldToScreen(xy.GetTopLeft()), WorldToScreen(xy.GetTopRight()),
					 WorldToScreen(xy.GetBottomRight()), WorldToScreen(xy.GetBottomLeft())};
	m_gameState.tPosition = WorldToScreen(xy.GetCentre());
	return this;
}

Quad* Quad::SetTexture(TextureAsset& texture)
{
	m_quadGameState.pTexture = &texture;
	return this;
}

Quad* Quad::SetUV(Rect2 uv, bool bImmediate)
{
	if (bImmediate)
	{
		m_quadGameState.tUV.Reset(uv);
	}
	else
	{
		m_quadGameState.tUV.Update(uv);
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

Quad* Quad::SetUV(Fixed u, Fixed v, Fixed du, Fixed dv)
{
	return SetUV(Rect2::TLSize({du - u, dv - v}, {u, v}));
}
} // namespace LittleEngine
