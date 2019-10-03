#include "Core/Utils.h"
#include "SFMLAPI/Rendering/Primitives/Quads.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Rendering/LERenderer.h"
#include "../GameManager.h"
#include "Camera.h"
#include "TileMap.h"

namespace LittleEngine
{
Camera::Camera() : GameObject("Camera", "Camera"), m_orgWorldHeight(g_pGFX->WorldHeight()) {}

Camera::~Camera() = default;

void Camera::Shake(Fixed intensity, Time duration)
{
	if (!m_bShaking)
	{
		m_bShaking = true;
	}
	m_shakeIntensity = intensity;
	m_shakeDuration = duration;
	m_shakeElapsed = Time::Zero;
}

void Camera::FillViewWithTiles(TextureAsset& texture)
{
	m_pTexture = &texture;
	LERenderer* pRenderer = g_pGameManager->Renderer();
	if (!m_uTileMap)
	{
		m_uTileMap = MakeUnique<TileMap>(*pRenderer->New<Quads>(LayerID::Background), true);
	}
	m_uTileMap->FillSpace(g_pGFX->WorldSpace(), *m_pTexture);
}

void Camera::ClearTiles()
{
	m_uTileMap = nullptr;
	m_pTexture = nullptr;
}

Fixed Camera::Zoom() const
{
	return m_zoom;
}

void Camera::SetZoom(Fixed zoom, bool bImmediate)
{
	if (zoom < Fixed::Zero)
	{
		LOG_E("[Camera] Cannot set negative zoom! (%.2f)", zoom.ToF32());
		return;
	}
	m_zoom = zoom;
	g_pGFX->SetWorldHeight(m_orgWorldHeight * m_zoom, bImmediate);
}

Vector2 Camera::Position() const
{
	return m_position;
}

void Camera::SetPosition(Vector2 position)
{
	m_position = position;
}

void Camera::Reset()
{
	m_position = Vector2::Zero;
	SetZoom(Fixed::One, true);
	g_pGFX->SetWorldPosition(m_position, true);
}

void Camera::Tick(Time dt)
{
	if (m_bShaking)
	{
		m_shakeElapsed += dt;
		if (m_shakeElapsed >= m_shakeDuration)
		{
			m_bShaking = false;
			g_pGFX->SetWorldPosition(m_position, false);
		}
		else
		{
			Fixed x = Maths::Random::Range(Fixed::Zero, m_shakeIntensity);
			Fixed y = Maths::Random::Range(Fixed::Zero, m_shakeIntensity);
			g_pGFX->SetWorldPosition(m_position + Vector2(x, y), false);
		}
	}
	else
	{
		g_pGFX->SetWorldPosition(m_position, false);
	}
}
} // namespace LittleEngine
