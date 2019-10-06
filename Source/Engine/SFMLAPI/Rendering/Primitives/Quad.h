#pragma once
#include <array>
#include "SFML/Graphics/VertexArray.hpp"
#include "Primitive.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
class Quad : public APrimitive
{
public:
	using Verts = std::array<Vector2, 4>;

protected:
	
	struct QuadState
	{
		Verts verts;
		TRange<Rect2> tUV = Rect2::UV;
		TRange<Rect2> tModel = Rect2::Zero;
		class TextureAsset* pTexture = nullptr;
	};

protected:
	sf::VertexArray m_sfVertArr;
	QuadState m_quadGameState;
	QuadState m_quadRenderState;

public:
	Quad(LayerID layer);
	~Quad() override;

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
	Quad* SetModel(Rect2 xy, bool bImmediate);
	Quad* SetTexture(TextureAsset& texture);
	Quad* SetUV(Rect2 uv, bool bImmediate = false);
	Quad* SetUV(Fixed u, Fixed v, Fixed du, Fixed dv, bool bImmediate = false);

private:
	friend class Quads;
};
} // namespace LittleEngine
