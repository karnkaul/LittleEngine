#include "stdafx.h"
#include "SFPrimitive.h"
#include "Core/Logger.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
namespace
{
const sf::Vector2f ZERO = sf::Vector2f(0, 0);
}

SFPrimitive::SFPrimitive(LayerID layer) : m_layer(layer)
{
}

void SFPrimitive::SwapState()
{
	m_renderState = m_gameState;
	m_quadVec.SwapState();
}

SFPrimitive* SFPrimitive::SetEnabled(bool bEnabled)
{
	m_gameState.bEnabled = bEnabled;
	return this;
}

SFPrimitive* SFPrimitive::SetPosition(Vector2 worldPosition, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfPosition.Reset(WorldToScreen(worldPosition));
	}
	else
	{
		m_gameState.sfPosition.Update(WorldToScreen(worldPosition));
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetOrientation(Fixed worldOrientation, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfOrientation.Reset(WorldToScreen(worldOrientation));
	}
	else
	{
		m_gameState.sfOrientation.Update(WorldToScreen(worldOrientation));
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetScale(Vector2 worldScale, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfScale.Reset(worldScale);
	}
	else
	{
		m_gameState.sfScale.Update(worldScale);
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetPrimaryColour(Colour sfColour, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfPrimaryColour.Reset(sfColour);
	}
	else
	{
		m_gameState.sfPrimaryColour.Update(sfColour);
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetSecondaryColour(Colour sfColour, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfSecondaryColour.Reset(sfColour);
	}
	else
	{
		m_gameState.sfSecondaryColour.Update(sfColour);
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetPivot(Vector2 pivot)
{
	m_gameState.pivot = pivot;
	if (m_bStatic || m_bMakeStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetOutline(Fixed thickness)
{
	m_gameState.outlineThickness = thickness;
	if (m_bStatic || m_bMakeStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetSize(Vector2 size, SFShapeType onShape)
{
	m_gameState.shape = onShape;
	m_gameState.shapeSize = size;
	if (m_bStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	m_flags |= SHAPE;
	return this;
}

SFPrimitive* SFPrimitive::SetTexture(const TextureAsset& texture)
{
	m_gameState.pTexture = &texture;
	m_gameState.texRect.Import(*m_gameState.pTexture);
	if (m_bStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	m_flags |= SPRITE;
	return this;
}

SFPrimitive* SFPrimitive::CropTexture(SFTexRect textureRect)
{
	m_gameState.texRect = textureRect;
	if (m_bStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetUV(Fixed u, Fixed v, Fixed du, Fixed dv)
{
	Assert(m_gameState.pTexture, "Texture not present to compute TexRect!");
	Vector2 size = m_gameState.pTexture->GetTextureSize();
	Vector2 min(u * size.x, v * size.y);
	Vector2 max(du * size.x, dv * size.y);
	return CropTexture(SFTexRect(SFTexCoords(min), SFTexCoords(max)));
}

SFPrimitive* SFPrimitive::SetFont(const FontAsset& font)
{
	m_gameState.pFont = &font;
	m_bTextChanged.store(true, std::memory_order_relaxed);
	if (m_bStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetTextSize(u32 pixelSize)
{
	m_gameState.textSize = pixelSize;
	m_bTextChanged.store(true, std::memory_order_relaxed);
	if (m_bStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetText(String text)
{
	m_gameState.text = std::move(text);
	m_bTextChanged.store(true, std::memory_order_relaxed);
	if (m_bStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	m_flags |= TEXT;
	return this;
}

Rect2 SFPrimitive::GetBounds() const
{
	if (m_flags & SHAPE)
	{
		return GetShapeBounds();
	}

	if (m_flags & TEXT)
	{
		return GetTextBounds();
	}

	if (m_flags & SPRITE)
	{
		return GetSpriteBounds();
	}

	return {};
}

Rect2 SFPrimitive::GetShapeBounds() const
{
	float cSize = m_circle.getRadius() * 2;
	if (cSize > 0)
	{
		Vector2 pivot = Cast(m_circle.getOrigin());
		Vector2 size(cSize, cSize);
		Vector2 offset = Fixed::OneHalf * size;
		return Rect2::CentreSize(size, pivot + offset);
	}
	Vector2 pivot = Cast(m_rectangle.getOrigin());
	Vector2 size = Cast(m_rectangle.getSize());
	Vector2 offset = Fixed::OneHalf * size;
	return Rect2::CentreSize(size, pivot + offset);
}

Rect2 SFPrimitive::GetSpriteBounds() const
{
	sf::FloatRect bounds = m_sprite.getLocalBounds();
	Vector2 size(Fixed(bounds.width), Fixed(bounds.height));
	Vector2 offset = Fixed::OneHalf * size;
	Vector2 pivot = Cast(m_sprite.getOrigin());
	return Rect2::CentreSize(size, pivot + offset);
}

Rect2 SFPrimitive::GetTextBounds() const
{
	sf::FloatRect bounds = m_text.getLocalBounds();
	Vector2 size(Fixed(bounds.width), Fixed(bounds.height));
	Vector2 offset = Fixed::OneHalf * size;
	Vector2 pivot = Cast(m_text.getOrigin());
	return Rect2::CentreSize(size, pivot + offset);
}

bool SFPrimitive::IsEnabled() const
{
	return m_gameState.bEnabled;
}

bool SFPrimitive::IsStatic() const
{
	return m_bStatic;
}

Vector2 SFPrimitive::GetPosition() const
{
	return ScreenToWorld(m_gameState.sfPosition.max);
}

Fixed SFPrimitive::GetOrientation() const
{
	return ScreenToWorld(m_gameState.sfOrientation.max);
}

Vector2 SFPrimitive::GetScale() const
{
	return m_gameState.sfScale.max;
}

Colour SFPrimitive::GetPrimaryColour() const
{
	return m_gameState.sfPrimaryColour.max;
}

Colour SFPrimitive::GetSecondaryColour() const
{
	return m_gameState.sfSecondaryColour.max;
}

LayerID SFPrimitive::GetLayer() const
{
	return m_layer;
}

void SFPrimitive::ReconcileState()
{
	m_gameState.Reconcile();
	m_quadVec.Reconcile();
}

SFPrimitive* SFPrimitive::SetStatic(bool bStatic)
{
	if (bStatic)
	{
		m_bMakeStatic = true;
		m_bStatic = false;
	}
	else
	{
		m_bMakeStatic = m_bStatic = false;
	}
	return this;
}

SFPrimitive* SFPrimitive::SetShader(SFShader* pShader)
{
	m_pShader = pShader;
	return this;
}

SFQuadVec* SFPrimitive::GetQuadVec()
{
	return &m_quadVec;
}

void SFPrimitive::UpdateRenderState(Fixed alpha)
{
	if (!m_bWasDisabled && !m_renderState.bEnabled)
	{
		m_bWasDisabled = true;
		m_prevScale = Cast(m_renderState.sfScale.max);
		if (m_flags & SHAPE)
		{
			m_circle.setScale(ZERO);
			m_rectangle.setScale(ZERO);
		}
		else if (m_flags & TEXT)
		{
			m_text.setCharacterSize(0);
		}
		else if (m_flags & SPRITE)
		{
			m_sprite.setScale(ZERO);
		}
	}
	if (m_bWasDisabled && m_renderState.bEnabled)
	{
		m_bWasDisabled = false;
		if (m_flags & SHAPE)
		{
			m_circle.setScale(m_prevScale);
			m_rectangle.setScale(m_prevScale);
		}
		else if (m_flags & TEXT)
		{
			m_text.setCharacterSize(m_renderState.textSize);
		}
		else if (m_flags & SPRITE)
		{
			m_sprite.setScale(m_prevScale);
		}
	}
	if (m_bStatic || !m_renderState.bEnabled)
	{
		return;
	}

	// Set sizes, texts, and textures
	if (m_flags & SHAPE)
	{
		sf::Vector2f shapeSize = Cast(m_renderState.shapeSize);
		switch (m_renderState.shape)
		{
		default:
			break;

		case SFShapeType::Circle:
			m_circle.setRadius(shapeSize.x);
			break;

		case SFShapeType::Rectangle:
			m_rectangle.setSize(shapeSize);
			break;
		}
	}
	else if (m_flags & SPRITE)
	{
		m_sprite.setTexture(m_gameState.pTexture->m_sfTexture);
		m_sprite.setTextureRect(m_renderState.texRect.ToSFIntRect());
	}
	else if (m_flags & TEXT)
	{
		if (m_bTextChanged.load(std::memory_order_relaxed))
		{
			sf::String text = m_renderState.text;
			u32 textSize = m_renderState.textSize;
			m_text.setFont(m_gameState.pFont->m_sfFont);
			m_text.setString(text);
			m_text.setCharacterSize(textSize);
			m_bTextChanged.store(false, std::memory_order_relaxed);
		}
	}

	// Need size/text/texture set for GetBounds()
	Vector2 offset = Fixed::OneHalf * GetBounds().GetSize();
	Vector2 pivot = Vector2(offset.x * m_renderState.pivot.x, offset.y * m_renderState.pivot.y);
	sf::Vector2f origin = Cast(WorldToScreen(pivot) + offset);
	sf::Vector2f scale = Cast(m_renderState.sfScale.Lerp(alpha));
	f32 orientation = Cast(m_renderState.sfOrientation.Lerp(alpha));
	sf::Vector2f position = Cast(m_renderState.sfPosition.Lerp(alpha));
	sf::Color fill =
		Cast(Colour::Lerp(m_renderState.sfPrimaryColour.min, m_renderState.sfPrimaryColour.max, alpha));
	sf::Color outline =
		Cast(Colour::Lerp(m_renderState.sfSecondaryColour.min, m_renderState.sfSecondaryColour.max, alpha));
	f32 outlineThickness = Cast(m_renderState.outlineThickness);

	if (m_flags & SHAPE)
	{
		switch (m_renderState.shape)
		{
		default:
			break;

		case SFShapeType::Circle:
			m_circle.setOrigin(origin);
			m_circle.setScale(scale);
			m_circle.setRotation(orientation);
			m_circle.setPosition(position);
			m_circle.setFillColor(fill);
			m_circle.setOutlineThickness(outlineThickness);
			m_circle.setOutlineColor(outline);
			break;

		case SFShapeType::Rectangle:
			m_rectangle.setOrigin(origin);
			m_rectangle.setScale(scale);
			m_rectangle.setRotation(orientation);
			m_rectangle.setPosition(position);
			m_rectangle.setFillColor(fill);
			m_rectangle.setOutlineThickness(outlineThickness);
			m_rectangle.setOutlineColor(outline);
			break;
		}
	}
	else if (m_flags & SPRITE)
	{
		m_sprite.setOrigin(origin);
		m_sprite.setScale(scale);
		m_sprite.setRotation(orientation);
		m_sprite.setPosition(position);
		m_sprite.setColor(fill);
	}
	else if (m_flags & TEXT)
	{
		m_text.setOrigin(origin);
		m_text.setScale(scale);
		m_text.setRotation(orientation);
		m_text.setPosition(position);
		m_text.setFillColor(fill);
		m_text.setOutlineThickness(outlineThickness);
		m_text.setOutlineColor(outline);
	}

	if (m_bMakeStatic)
	{
		m_bStatic = true;
		m_bMakeStatic = false;
	}
	
	m_bRendered = true;
}

void SFPrimitive::Destroy()
{
	m_bDestroyed = true;
}
} // namespace LittleEngine
