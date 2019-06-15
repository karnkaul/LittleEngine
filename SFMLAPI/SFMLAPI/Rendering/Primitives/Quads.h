#pragma once
#include "Primitive.h"

namespace LittleEngine
{
class Quads : public APrimitive
{
public:
	Vec<UPtr<class Quad>> m_quads;
	class TextureAsset* m_pTexture = nullptr;

public:
	Quads(LayerID layer);
	~Quads() override;

public:
	Rect2 GetBounds() const override;

public:
	void ReconcileGameState() override;
	void SwapState() override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(SFViewport& viewport, sf::RenderStates& sfStates) override;

public:
	Quads* SetTexture(TextureAsset& texture);
	Quad* AddQuad();

	bool IsPopulated() const;
};
} // namespace LittleEngine