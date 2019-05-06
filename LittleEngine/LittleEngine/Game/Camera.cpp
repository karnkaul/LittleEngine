#include "stdafx.h"
#include "Core/Utils.h"
#include "Camera.h"

namespace LittleEngine
{
Camera::Camera() = default;
Camera::~Camera() = default;

void Camera::Shake(Fixed intensity, Time duration)
{
	if (!m_bShaking)
	{
		m_prevPosition = m_transform.localPosition;
		m_bShaking = true;
	}
	m_shakeIntensity = intensity;
	m_shakeDuration = duration;
	m_shakeElapsed = Time::Zero;
}

void Camera::Tick(Time dt)
{
	if (m_bShaking)
	{
		m_shakeElapsed += dt;
		if (m_shakeElapsed >= m_shakeDuration)
		{
			m_bShaking = false;
			m_transform.localPosition = m_prevPosition;
		}
		else
		{
			Fixed x = Maths::Random::Range(Fixed::Zero, m_shakeIntensity);
			Fixed y = Maths::Random::Range(Fixed::Zero, m_shakeIntensity);
			m_transform.localPosition = m_prevPosition + Vector2(x, y);
		}
	}
}
} // namespace LittleEngine