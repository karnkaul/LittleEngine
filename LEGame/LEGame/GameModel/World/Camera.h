#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
class Camera
{
public:
	using Transform = Core::Transform;

public:
	Transform m_transform;

private:
	UPtr<class TileMap> m_uTileMap;
	Vector2 m_prevPosition;
	Time m_shakeDuration;
	Time m_shakeElapsed;
	Fixed m_shakeIntensity;
	class LEContext* m_pContext;
	bool m_bShaking = false;

public:
	Camera(LEContext& context);
	virtual ~Camera();

	virtual void Shake(Fixed intensity = Fixed(5), Time duration = Time::Seconds(0.5f));
	void FillViewWithTiles(class TextureAsset& texture);
	void ClearTiles();

protected:
	virtual void Tick(Time dt);

private:
	friend class GameManager;
};
} // namespace LittleEngine
