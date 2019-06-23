#pragma once
#include "SFML/Graphics/VertexArray.hpp"
#include "Primitive.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
class Quad : public APrimitive
{
protected:
	struct QuadState
	{
		TRange<Rect2> tUV = Rect2::UV;
		class TextureAsset* pTexture = nullptr;
	};

protected:
	Array<Vector2, 4> m_vertexModel;
	sf::VertexArray m_sfVertArr;
	QuadState m_quadGameState;
	QuadState m_quadRenderState;

public:
	Quad(LayerID layer);
	~Quad() override;

public:
	Rect2 GetBounds() const override;

public:
	void ReconcileGameState() override;
	void SwapState() override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnDraw(Viewport& viewport, sf::RenderStates& sfStates) override;

public:
	Quad* SetModel(Rect2 xy);
	Quad* SetTexture(TextureAsset& texture);
	Quad* SetUV(Rect2 uv, bool bImmediate = false);
	Quad* SetUV(Fixed u, Fixed v, Fixed du, Fixed dv, bool bImmediate = false);

private:
	friend class Quads;
};
} // namespace LittleEngine