#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/LayerID.h"

namespace LittleEngine
{
struct TileData
{
	class TextureAsset* pTexture = nullptr;
	u16 xRows = 0;
	u16 xCols = 0;

	void FillSpace(Vector2 space, TextureAsset& texture);
};

class TileMap
{
private:
	std::vector<class Quad*> m_quads;
	class Quads* m_pQuads;
	bool m_bDestroyPrimitive;

public:
	TileMap(Quads& quad, bool bAutoDestroyPrimitive = true);
	virtual ~TileMap();

	void CreateTiles(TileData data);
	void FillSpace(Vector2 space, TextureAsset& texture);
};
} // namespace LittleEngine
