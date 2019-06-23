#include "stdafx.h"
#include "Core/Utils.h"
#include "SFMLAPI/Rendering/Primitives/Quads.h"
#include "SFMLAPI/System/Assets.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "../GameManager.h"
#include "Camera.h"
#include "TileMap.h"

namespace LittleEngine
{
Camera::Camera() : GameObject("Camera", "Camera")
{
}

Camera::~Camera() = default;

void Camera::Shake(Fixed intensity, Time duration)
{
	if (!m_bShaking)
	{
		m_prevPosition = m_transform.GetPosition();
		m_bShaking = true;
	}
	m_shakeIntensity = intensity;
	m_shakeDuration = duration;
	m_shakeElapsed = Time::Zero;
}

void Camera::FillViewWithTiles(class TextureAsset& texture)
{
	LERenderer* pRenderer = g_pGameManager->Renderer();
	if (!m_uTileMap)
	{
		m_uTileMap = MakeUnique<TileMap>(*pRenderer->New<Quads>(LAYER_ZERO), true);
	}
	m_uTileMap->FillView(pRenderer->GetViewSize(), texture);
}

void Camera::ClearTiles()
{
	m_uTileMap = nullptr;
}

void Camera::Tick(Time dt)
{
	if (m_bShaking)
	{
		m_shakeElapsed += dt;
		if (m_shakeElapsed >= m_shakeDuration)
		{
			m_bShaking = false;
			m_transform.SetPosition(m_prevPosition);
		}
		else
		{
			Fixed x = Maths::Random::Range(Fixed::Zero, m_shakeIntensity);
			Fixed y = Maths::Random::Range(Fixed::Zero, m_shakeIntensity);
			m_transform.SetPosition(m_prevPosition + Vector2(x, y));
		}
	}
}
} // namespace LittleEngine