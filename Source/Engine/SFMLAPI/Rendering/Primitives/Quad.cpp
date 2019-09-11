#include "Core/Logger.h"
#include "SFML/Graphics/Vertex.hpp"
#include "Quad.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/System/SFTypes.h"
#if ENABLED(RENDER_STATS)
#include "SFMLAPI/Rendering/Renderer.h"
#endif

namespace LittleEngine
{
#if ENABLED(RENDER_STATS)
extern RenderData g_renderData;
#endif

Quad::Quad(LayerID layer) : APrimitive(layer), m_sfVertArr(sf::VertexArray(sf::Quads, 4)) {}

Quad::~Quad() = default;

Rect2 Quad::GameBounds() const
{
	Vector2 size(Maths::Abs(m_vertexModel[0].x - m_vertexModel[1].x), Maths::Abs(m_vertexModel[1].y - m_vertexModel[2].y));
	Vector2 centre = ViewportToWorld(m_gameState.tPosition.max);
	return Rect2::SizeCentre(size, centre);
}

Rect2 Quad::RenderBounds() const
{
	Vector2 size(Maths::Abs(m_vertexModel[0].x - m_vertexModel[1].x), Maths::Abs(m_vertexModel[1].y - m_vertexModel[2].y));
	Vector2 centre = ViewportToWorld(m_renderState.tPosition.max);
	return Rect2::SizeCentre(size, centre);
}

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
	// Cheap
	Assert(m_vertexModel.size() == 4, "Invalid vertex model for Quad!");
	State state = GetState(alpha);
	Colour c = state.colour;
	Rect2 uvRect = Maths::Lerp(m_quadRenderState.tUV.min, m_quadRenderState.tUV.max, alpha);
	Vector2 size = m_quadRenderState.pTexture ? m_quadRenderState.pTexture->TextureSize() : Vector2::Zero;
	Vector2 uvs[] = {uvRect.TopLeft(), uvRect.TopRight(), uvRect.BottomRight(), uvRect.BottomLeft()};
	size_t idx = 0;

	// Expensive
	Matrix3 transform(state.position, state.orientation, state.scale);
	for (auto& vertex : m_vertexModel)
	{
		Vector2 position = vertex * transform;
		Vector2 uv = {size.x * uvs[idx].x, size.y * uvs[idx].y};
		m_sfVertArr[idx++] = sf::Vertex(Cast(position), Cast(c), Cast(uv));
	}
}

void Quad::OnDraw(Viewport& viewport, sf::RenderStates& sfStates)
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
	m_vertexModel = {WorldToViewport(xy.TopLeft()), WorldToViewport(xy.TopRight()), WorldToViewport(xy.BottomRight()),
					 WorldToViewport(xy.BottomLeft())};
	m_gameState.tPosition = WorldToViewport(xy.Centre());
	return this;
}

Quad* Quad::SetTexture(TextureAsset& texture)
{
	Assert(&texture, "Texture is null");
	m_quadGameState.pTexture = &texture;
	SetUV(Fixed::Zero, Fixed::Zero, Fixed::One, Fixed::One, true);
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
	SetDirty(true);
	return this;
}

Quad* Quad::SetUV(Fixed u, Fixed v, Fixed du, Fixed dv, bool bImmediate)
{
	return SetUV(Rect2::SizeTL({du - u, dv - v}, {u, v}), bImmediate);
}
} // namespace LittleEngine
