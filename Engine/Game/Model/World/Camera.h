#pragma once
#include "GameObject.h"

namespace LittleEngine
{
class Camera : public GameObject
{
private:
	std::unique_ptr<class TileMap> m_uTileMap;
	Vector2 m_position;
	Time m_shakeDuration;
	Time m_shakeElapsed;
	Fixed m_shakeIntensity;
	Fixed m_orgWorldHeight;
	Fixed m_zoom = Fixed::One;
	class TextureAsset* m_pTexture = nullptr;
	bool m_bShaking = false;

public:
	Camera();
	~Camera() override;

	virtual void Shake(Fixed intensity = Fixed(5), Time duration = Time::Seconds(0.5f));
	void FillViewWithTiles(TextureAsset& texture);
	void ClearTiles();

	Fixed Zoom() const;
	void SetZoom(Fixed zoom, bool bImmediate);
	Vector2 Position() const;
	void SetPosition(Vector2 position);

protected:
	virtual void Reset();
	virtual void Tick(Time dt);

private:
	friend class GameManager;
};
} // namespace LittleEngine
