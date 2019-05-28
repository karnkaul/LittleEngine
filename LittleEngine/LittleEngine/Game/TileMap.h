#pragma once
#include "LittleEngine/GFX/GFX.h"

namespace LittleEngine
{
struct TileData
{
	Vector2 startPos = Vector2::Zero;
	class TextureAsset* pTexture = nullptr;
	u8 rows = 0;
	u8 columns = 0;

	void FillView(TextureAsset& texture);
};

class TileMap
{
private:
	class SFPrimitive* m_pSFPrimitive;

public:
	TileMap(LayerID layer = LAYER_DEFAULT);
	virtual ~TileMap();

	void CreateTiles(TileData data);
	void FillView(TextureAsset& texture);
};
} // namespace LittleEngine