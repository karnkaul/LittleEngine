#pragma once
#include <deque>
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
	
public:
	std::deque<Quad> m_quads;
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
	Quads* SetTexture(TextureAsset& texture);
	Quad* AddQuad();

	bool IsPopulated() const;
};
} // namespace LittleEngine
