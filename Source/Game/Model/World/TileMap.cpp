#include "Core/Asserts.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "SFMLAPI/Rendering/Primitives/Quads.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Renderer/LERenderer.h"
#include "TileMap.h"

namespace LittleEngine
{
void TileData::FillSpace(Vector2 space, TextureAsset& texture)
{
	pTexture = &texture;
	Vector2 texSize = pTexture->TextureSize();
	Fixed xResidue = space.x - texSize.x;
	while (xResidue > Fixed::Zero)
	{
		++xCols;
		xResidue -= (2 * texSize.x);
	}
	Fixed yResidue = space.y - texSize.y;
	while (yResidue > Fixed::Zero)
	{
		++xRows;
		yResidue -= (2 * texSize.y);
	}
}

TileMap::TileMap(Quads& quad, bool bAutoDestroyPrimitive) : m_pQuads(&quad), m_bDestroyPrimitive(bAutoDestroyPrimitive) {}

TileMap::~TileMap()
{
	if (m_bDestroyPrimitive)
	{
		m_pQuads->Destroy();
	}
}

void TileMap::CreateTiles(TileData data)
{
	Assert(data.pTexture, "Creating Tiles without texture!");
	m_pQuads->SetTexture(*data.pTexture);
	Vector2 size = data.pTexture->TextureSize();

	Fixed x, y;
	size_t idx = 0;

	auto create = [&](Vector2 centre) {
		Quad* pQuad;
		if (idx < m_quads.size())
		{
			pQuad = m_quads[idx];
		}
		else
		{
			pQuad = m_pQuads->AddQuad();
			m_quads.push_back(pQuad);
		}
		++idx;
		pQuad->SetPosition(centre, true)->SetEnabled(true);
	};
	auto createRow = [&]() {
		x = Fixed::Zero;
		create({x, y});
		for (u16 col = 1; col <= data.xCols; ++col)
		{
			x = col * size.x;
			create({x, y});
			create({-x, y});
		}
	};
	// Centre
	createRow();
	for (u16 row = 1; row <= data.xRows; ++row)
	{
		y = row * size.y;
		createRow();
		y = -y;
		createRow();
	}
	m_pQuads->SetEnabled(true);
}

void TileMap::FillSpace(Vector2 space, TextureAsset& texture)
{
	TileData data;
	data.FillSpace(space, texture);
	CreateTiles(data);
}
} // namespace LittleEngine
