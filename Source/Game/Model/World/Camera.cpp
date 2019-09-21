#include "Core/Utils.h"
#include "SFMLAPI/Rendering/Primitives/Quads.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Renderer/LERenderer.h"
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
		m_prevPosition = m_transform.Position();
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
		m_uTileMap = MakeUnique<TileMap>(*pRenderer->New<Quads>(LayerID::Zero), true);
	}
	m_uTileMap->FillSpace(g_pGFX->WorldSpace(), *m_pTexture);
	m_bViewfilled = true;
}

void Camera::ClearTiles()
{
	m_uTileMap = nullptr;
	m_bViewfilled = false;
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
	if (m_uTileMap && m_pTexture && m_bViewfilled)
	{
		m_uTileMap->FillSpace(g_pGFX->WorldSpace(), *m_pTexture);
	}
}

void Camera::Reset()
{
	m_transform.SetPosition(Vector2::Zero);
	m_transform.SetOrientation(Vector2::Right);
	m_transform.SetScale(Vector2::One);
	SetZoom(Fixed::One, true);
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
