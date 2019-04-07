#pragma once
#include "SimpleTime.h"
#include "Transform.h"

namespace LittleEngine
{
class Camera
{
public:
	using Transform = Core::Transform;

public:
	Transform m_transform;

private:
	Vector2 m_prevPosition;
	Time m_shakeDuration;
	Time m_shakeElapsed;
	Fixed m_shakeIntensity;
	bool m_bShaking = false;

public:
	Camera();
	virtual ~Camera();

	virtual void Shake(Fixed intensity = Fixed(5), Time duration = Time::Seconds(0.5f));

protected:
	virtual void Tick(Time dt);

private:
	friend class GameManager;
};
} // namespace LittleEngine
