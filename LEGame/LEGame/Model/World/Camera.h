#pragma once
#include "GameObject.h"

namespace LittleEngine
{
class Camera : public GameObject
{
public:
	Transform m_transform;

private:
	UPtr<class TileMap> m_uTileMap;
	Vector2 m_prevPosition;
	Time m_shakeDuration;
	Time m_shakeElapsed;
	Fixed m_shakeIntensity;
	bool m_bShaking = false;

public:
	Camera();
	~Camera() override;

	virtual void Shake(Fixed intensity = Fixed(5), Time duration = Time::Seconds(0.5f));
	void FillViewWithTiles(class TextureAsset& texture);
	void ClearTiles();

protected:
	virtual void Tick(Time dt);

private:
	friend class GameManager;
};
} // namespace LittleEngine
