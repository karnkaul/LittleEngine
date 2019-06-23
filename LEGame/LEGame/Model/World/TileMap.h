#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/LayerID.h"

namespace LittleEngine
{
struct TileData
{
	Vector2 startPos = Vector2::Zero;
	class TextureAsset* pTexture = nullptr;
	u8 rows = 0;
	u8 columns = 0;

	void FillView(Vector2 viewSize, TextureAsset& texture);
};

class TileMap
{
private:
	class Quads* m_pQuads;
	bool m_bDestroyPrimitive;

public:
	TileMap(Quads& quad, bool bAutoDestroyPrimitive = true);
	virtual ~TileMap();

	void CreateTiles(TileData data);
	void FillView(Vector2 viewSize, TextureAsset& texture);
};
} // namespace LittleEngine