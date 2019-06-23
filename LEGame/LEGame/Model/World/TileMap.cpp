#include "stdafx.h"
#include "Core/Asserts.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "SFMLAPI/Rendering/Primitives/Quads.h"
#include "SFMLAPI/System/Assets.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "TileMap.h"

namespace LittleEngine
{
void TileData::FillView(Vector2 viewSize, TextureAsset& texture)
{
	pTexture = &texture;
	Vector2 texSize = pTexture->GetTextureSize();
	Vector2 halfTexSize = Fixed::OneHalf * texSize;
	startPos = {-Fixed::OneHalf * viewSize.x, Fixed::OneHalf * viewSize.y};
	startPos.x += halfTexSize.x;
	startPos.y -= halfTexSize.y;
	columns = (viewSize.x / texSize.x).ToS32();
	if (viewSize.x.ToS32() % texSize.x.ToS32() > 0)
	{
		++columns;
	}
	rows = (viewSize.y / texSize.y).ToS32();
	if (viewSize.y.ToS32() % texSize.y.ToS32() > 0)
	{
		++rows;
	}
}

TileMap::TileMap(Quads& quad, bool bAutoDestroyPrimitive)
	: m_pQuads(&quad), m_bDestroyPrimitive(bAutoDestroyPrimitive)
{
}

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
	Vector2 textureSize = data.pTexture->GetTextureSize();
	Vector2 centre = data.startPos;
	for (u16 row = 0; row < data.rows; ++row)
	{
		for (u16 col = 0; col < data.columns; ++col)
		{
			Quad* pQuad = m_pQuads->AddQuad();
			pQuad->SetPosition(centre)->SetEnabled(true);
			centre.x += textureSize.x;
		}
		centre.x = data.startPos.x;
		centre.y -= textureSize.y;
	}
	m_pQuads->SetEnabled(true);
}

void TileMap::FillView(Vector2 viewSize, TextureAsset& texture)
{
	TileData data;
	data.FillView(viewSize, texture);
	CreateTiles(data);
}
} // namespace LittleEngine