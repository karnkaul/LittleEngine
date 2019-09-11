#pragma once
#include "Primitive.h"
#include "Quad.h"

namespace LittleEngine
{
class Quads : public APrimitive
{
public:
#ifdef DEBUGGING
	static bool s_bUSE_JOBS;
#endif
	static const u32 s_JOB_QUAD_LIMIT = 1024;
	static const u32 s_QUAD_COUNT = 4096;

private:
	u32 m_reserved = 0;

public:
	Vec<Quad> m_quads;
	class TextureAsset* m_pTexture = nullptr;

public:
	Quads(LayerID layer);
	~Quads() override;

public:
	Rect2 GameBounds() const override;
	Rect2 RenderBounds() const override;

public:
	void ReconcileGameState() override;
	void SwapState() override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(Viewport& viewport, sf::RenderStates& sfStates) override;

public:
	Quads* SetTexture(TextureAsset& texture, u32 maxQuadCount = s_QUAD_COUNT);
	Quad* AddQuad();

	bool IsPopulated() const;
};
} // namespace LittleEngine
