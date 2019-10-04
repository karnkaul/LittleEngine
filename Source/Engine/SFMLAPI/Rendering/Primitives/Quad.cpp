#include "Core/Logger.h"
#include "SFML/Graphics/Vertex.hpp"
#include "Quad.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/System/SFTypes.h"
#if ENABLED(RENDER_STATS)
#include "SFMLAPI/Rendering/RenderStats.h"
#endif

namespace LittleEngine
{
namespace
{
void Lerp(Quad::Verts& out, const Core::TRange<Rect2>& tModel, Fixed alpha)
{
	Rect2 model = Maths::Lerp(tModel.min, tModel.max, alpha);
	out[0] = model.TopLeft();
	out[1] = model.TopRight();
	out[2] = model.BottomRight();
	out[3] = model.BottomLeft();
}
} // namespace

Quad::Quad(LayerID layer) : APrimitive(layer), m_sfVertArr(sf::VertexArray(sf::Quads, 4)) {}

Quad::~Quad() = default;

Rect2 Quad::GameBounds() const
{
	Vector2 centre = SFMLToWorld(m_gameState.tPosition.max);
	return Rect2::SizeCentre(m_quadGameState.tModel.max.Size(), centre);
}

Rect2 Quad::RenderBounds() const
{
	Vector2 centre = SFMLToWorld(m_renderState.tPosition.max);
	return Rect2::SizeCentre(m_quadRenderState.tModel.max.Size(), centre);
}

void Quad::ReconcileGameState()
{
	APrimitive::ReconcileGameState();
	m_quadGameState.tUV.min = m_quadGameState.tUV.max;
	m_quadGameState.tModel.min = m_quadGameState.tModel.max;
}

void Quad::SwapState()
{
	APrimitive::SwapState();
	m_quadRenderState = m_quadGameState;
}

void Quad::OnUpdateRenderState(Fixed alpha)
{
	// Cheap
	Assert(m_quadRenderState.verts.size() == 4, "Invalid vertex model for Quad!");
	State state = GetState(alpha);
	Colour c = state.colour;
	Rect2 uvRect = Maths::Lerp(m_quadRenderState.tUV.min, m_quadRenderState.tUV.max, alpha);
	Vector2 size = m_quadRenderState.pTexture ? m_quadRenderState.pTexture->TextureSize() : Vector2::Zero;
	Vector2 uvs[] = {uvRect.TopLeft(), uvRect.TopRight(), uvRect.BottomRight(), uvRect.BottomLeft()};
	size_t idx = 0;

	// Expensive
	Matrix3 transform(state.position, state.orientation, state.scale);
	Lerp(m_quadRenderState.verts, m_quadRenderState.tModel, alpha);
	for (auto& vertex : m_quadRenderState.verts)
	{
		Vector2 p = vertex * transform;
		Vector2 uv = {size.x * uvs[idx].x, size.y * uvs[idx].y};
		sf::Vertex& v = m_sfVertArr[idx++];
		v.position = Cast(p);
		v.color = Cast(c);
		v.texCoords = Cast(uv);
	}
}

void Quad::OnDraw(Viewport& viewport, sf::RenderStates& sfStates)
{
#if ENABLED(RENDER_STATS)
	++g_renderStats._quadCount_Internal;
#endif
	if (m_quadRenderState.pTexture)
	{
		sfStates.texture = &m_quadRenderState.pTexture->m_sfTexture;
	}
	viewport.draw(m_sfVertArr, sfStates);
}

Quad* Quad::SetModel(Rect2 xy, bool bImmediate)
{
	if (bImmediate)
	{
		m_quadGameState.tModel.Reset(xy);
	}
	else
	{
		m_quadGameState.tModel.Update(xy);
	}
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
