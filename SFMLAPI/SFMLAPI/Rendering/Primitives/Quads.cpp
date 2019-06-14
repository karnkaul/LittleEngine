#include "stdafx.h"
#include "Quads.h"
#include "Quad.h"
#include "SFMLAPI/System/SFAssets.h"

namespace LittleEngine
{
Quads::Quads(LayerID layer) : APrimitive(layer)
{
	m_quads.reserve(1024);
}

Quads::~Quads() = default;

void Quads::OnUpdateRenderState(Fixed alpha)
{
	for (auto& uQuad : m_quads)
	{
		uQuad->UpdateRenderState(alpha);
	}
}

void Quads::OnDraw(SFViewport& viewport, sf::RenderStates& sfStates)
{
	Vec<Quad*> enabledQuads;
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
	for (auto& pQuad : enabledQuads)
	{
		for (size_t i = 0; i < pQuad->m_sfVertArr.getVertexCount(); ++i)
		{
			va[idx++] = pQuad->m_sfVertArr[i];
		}
	}
	sfStates.texture = &m_pTexture->m_sfTexture;
	viewport.draw(va, sfStates);
}

void Quads::OnSwapState()
{
	for (auto& uQuad : m_quads)
	{
		uQuad->SwapState();
	}
}

void Quads::ReconcileGameState()
{
	APrimitive::ReconcileGameState();
	for (auto& uQuad : m_quads)
	{
		uQuad->ReconcileGameState();
	}
}

Rect2 Quads::GetBounds() const
{
	return Rect2::Zero;
}

Quads* Quads::SetTexture(TextureAsset& texture)
{
	m_pTexture = &texture;
	return this;
}

Quad* Quads::AddQuad()
{
	Assert(m_pTexture, "Texture is null!");
	if (m_pTexture)
	{
		UPtr<Quad> uQuad = MakeUnique<Quad>(m_layer);
		uQuad->SetModel(Rect2::CentreSize(m_pTexture->GetTextureSize()))->SetTexture(*m_pTexture);
		Quad* pQuad = uQuad.get();
		m_quads.emplace_back(std::move(uQuad));
		return pQuad;
	}
	return nullptr;
}

bool Quads::IsPopulated() const
{
	return !m_quads.empty();
}
} // namespace LittleEngine