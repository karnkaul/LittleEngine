#pragma once
#include "SFML/Graphics/VertexArray.hpp"
#include "Primitive.h"
#include "SFMLAPI/Rendering/Colour.h"

namespace LittleEngine
{
struct Vertex
{
	Vector2 xy;
	Vector2 uv;
	Colour colour;
};

class Quad : public APrimitive
{
protected:
	struct QuadState
	{
		class TextureAsset* pTexture = nullptr;
	};

protected:
	Array<Vertex, 4> m_vertices;
	sf::VertexArray m_sfVertArr;
	QuadState m_quadGameState;
	QuadState m_quadRenderState;

public:
	Quad(LayerID layer);
	~Quad() override;

protected:
	void OnUpdateRenderState(Fixed alpha) override;
	void OnSwapState() override;
	void OnDraw(SFViewport& viewport, sf::RenderStates& sfStates) override;

public:
	Rect2 GetBounds() const override;

public:
	Quad* SetModel(Rect2 xy, Rect2 uv = Rect2::UV, Colour colour = Colour::White);
	Quad* SetTexture(TextureAsset& texture);
	Quad* SetUV(Rect2 uv);
	Quad* SetUV(Fixed u, Fixed v, Fixed du, Fixed dv);

	Matrix3 GetMVP(Fixed alpha) const;

private:
	friend class Quads;
};
}