#include "stdafx.h"
#include "Core/Jobs.h"
#include "Core/Logger.h"
#include "Quads.h"
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

Quads::Quads(LayerID layer) : APrimitive(layer)
{
}

Quads::~Quads() = default;

Rect2 Quad::GetBounds() const
{
	Vector2 size(Maths::Abs(m_vertexModel[0].x - m_vertexModel[1].x),
				 Maths::Abs(m_vertexModel[1].y - m_vertexModel[2].y));
	return Rect2::CentreSize(size);
}

void Quads::ReconcileGameState()
{
	APrimitive::ReconcileGameState();
	for (auto& quad : m_quads)
	{
		quad.ReconcileGameState();
	}
}

void Quads::SwapState()
{
	APrimitive::SwapState();
	for (auto& quad : m_quads)
	{
		quad.SwapState();
	}
}

void Quads::OnUpdateRenderState(Fixed alpha)
{
	if (m_quads.size() < s_JOB_QUAD_LIMIT || !s_bUSE_JOBS)
	{
		for (auto& quad : m_quads)
		{
			quad.UpdateRenderState(alpha);
		}
	}
	else
	{
		Core::Jobs::ForEach([&](size_t idx) { m_quads[idx].UpdateRenderState(alpha); },
							m_quads.size(), s_JOB_QUAD_LIMIT);
	}
}

void Quads::OnDraw(SFViewport& viewport, sf::RenderStates& sfStates)
{
	size_t enabledCount = 0;
	for (const auto& quad : m_quads)
	{
		if (quad.m_renderState.bEnabled)
		{
			++enabledCount;
		}
	}

	sf::VertexArray va(sf::Quads, enabledCount * 4);
	size_t idx = 0;
	for (const auto& quad : m_quads)
	{
		if (quad.m_renderState.bEnabled)
		{
			for (size_t i = 0; i < quad.m_sfVertArr.getVertexCount(); ++i)
			{
				va[idx++] = quad.m_sfVertArr[i];
			}
		}
	}
#if ENABLED(RENDER_STATS)
	g_renderData._quadCount_Internal += enabledCount;
#endif
	sfStates.texture = &m_pTexture->m_sfTexture;
	viewport.draw(va, sfStates);
}

Rect2 Quads::GetBounds() const
{
	return Rect2::Zero;
}

Quads* Quads::SetTexture(TextureAsset& texture, u32 maxQuadCount)
{
	if (m_quads.capacity() == 0)
	{
		m_quads.reserve(maxQuadCount);
		m_reserved = m_quads.capacity();
	}
	m_pTexture = &texture;
	return this;
}

Quad* Quads::AddQuad()
{
	Assert(m_pTexture, "Texture is null!");
	Assert(m_quads.size() < m_reserved, "Max quads reached!");
	if (m_pTexture && m_quads.size() < m_reserved)
	{
		Quad quad(m_layer);
		quad.SetModel(Rect2::CentreSize(m_pTexture->GetTextureSize()))->SetTexture(*m_pTexture);
		m_quads.emplace_back(std::move(quad));
		return &m_quads.back();
	}
	return nullptr;
}

bool Quads::IsPopulated() const
{
	return !m_quads.empty();
}
} // namespace LittleEngine