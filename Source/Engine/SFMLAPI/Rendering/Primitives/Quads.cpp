#include "Core/Jobs.h"
#include "Core/Logger.h"
#include "Quads.h"
#include "Quad.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/System/SFTypes.h"
#if ENABLED(RENDER_STATS)
#include "SFMLAPI/Rendering/PrimitiveFactory.h"
#endif

namespace LittleEngine
{
#if ENABLED(RENDER_STATS)
extern RenderData g_renderData;
#endif

#ifdef DEBUGGING
bool Quads::s_bUSE_JOBS = true;
#endif

Quads::Quads(LayerID layer) : APrimitive(layer) {}

Quads::~Quads() = default;

Rect2 Quads::GameBounds() const
{
	return Rect2::Zero;
}

Rect2 Quads::RenderBounds() const
{
	return Rect2::Zero;
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
	auto update = [alpha](Quad& quad) {
		if (quad.m_renderState.bEnabled)
		{
			quad.UpdateRenderState(alpha);
		}
	};
	if (m_quads.size() < s_JOB_QUAD_LIMIT
#ifdef DEBUGGING
		|| !s_bUSE_JOBS
#endif
	)
	{
		for (auto& quad : m_quads)
		{
			update(quad);
		}
	}
	else
	{
		Core::Jobs::ForEach(
			[&](size_t idx) {
				auto& quad = m_quads[idx];
				update(quad);
			},
			m_quads.size(), s_JOB_QUAD_LIMIT);
	}
}

void Quads::OnDraw(Viewport& viewport, sf::RenderStates& sfStates)
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
	g_renderData._quadCount_Internal += static_cast<u32>(enabledCount);
#endif
	sfStates.texture = &m_pTexture->m_sfTexture;
	viewport.draw(va, sfStates);
}

Quads* Quads::SetTexture(TextureAsset& texture, u32 maxQuadCount)
{
	Assert(&texture, "Texture is null!");
	if (m_quads.capacity() == 0)
	{
		m_quads.reserve(maxQuadCount);
		m_reserved = static_cast<u32>(m_quads.capacity());
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
		quad.SetModel(Rect2::SizeCentre(m_pTexture->TextureSize()), true)->SetTexture(*m_pTexture);
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
