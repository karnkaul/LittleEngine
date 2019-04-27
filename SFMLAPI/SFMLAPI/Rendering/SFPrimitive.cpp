#include "stdafx.h"
#include "SFPrimitive.h"
#include "Logger.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
namespace
{
const sf::Vector2f ZERO = sf::Vector2f(0, 0);
}

Vector2 SFPrimitive::WorldToScreen(Vector2 worldPoint)
{
	return Vector2(worldPoint.x, -worldPoint.y);
}

Fixed SFPrimitive::WorldToScreen(Fixed worldOrientation)
{
	return -worldOrientation;
}

Vector2 SFPrimitive::ScreenToWorld(Vector2 screenPoint)
{
	return Vector2(screenPoint.x, -screenPoint.y);
}

Fixed SFPrimitive::ScreenToWorld(Fixed screenOrientation)
{
	return -screenOrientation;
}

SFPrimitive::SFPrimitive(LayerID layer)
{
	m_gameState.layer = layer;
	m_renderState.layer = layer;
	SetPivot(Vector2::Zero);
}

void SFPrimitive::SwapState()
{
	m_renderState = m_gameState;
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

SFPrimitive* SFPrimitive::SetPivot(Vector2 pivot)
{
	m_gameState.pivot = pivot;
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

SFPrimitive* SFPrimitive::SetOutline(Fixed thickness)
{
	m_gameState.outlineThickness = thickness;
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileState();
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
	return this;
}

SFPrimitive* SFPrimitive::SetTexture(const TextureAsset& texture)
{
	m_gameState.pTexture = &texture;
	m_sprite.setTexture(m_gameState.pTexture->m_sfTexture);
	return this;
}

SFPrimitive* SFPrimitive::CropTexture(SFTexRect textureRect)
{
	m_gameState.texRect = textureRect;
	m_sprite.setTextureRect(m_gameState.texRect.Cast());
	return this;
}

SFPrimitive* SFPrimitive::SetFont(const FontAsset& font)
{
	m_gameState.pFont = &font;
	m_text.setFont(m_gameState.pFont->m_sfFont);
	return this;
}

SFPrimitive* SFPrimitive::SetTextSize(u32 pixelSize)
{
	m_gameState.textSize = pixelSize;
	m_gameState.bTextSet = true;
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
	m_gameState.bTextSet = true;
	if (m_bStatic)
	{
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

Rect2 SFPrimitive::GetBounds() const
{
	Rect2 ret = GetShapeBounds();
	Rect2 temp = GetSpriteBounds();
	if (temp.GetSize().SqrMagnitude() > ret.GetSize().SqrMagnitude())
	{
		ret = temp;
	}
	temp = GetTextBounds();
	if (temp.GetSize().SqrMagnitude() > ret.GetSize().SqrMagnitude())
	{
		ret = temp;
	}
	return ret;
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
	return m_gameState.layer;
}

void SFPrimitive::ReconcileState()
{
	m_gameState.Reconcile();
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

void SFPrimitive::UpdatePivot()
{
	Vector2 offset = Fixed::OneHalf * GetBounds().GetSize();
	Vector2 pivot = Vector2(offset.x * m_gameState.pivot.x, offset.y * m_gameState.pivot.y);
	sf::Vector2f v = Cast(WorldToScreen(pivot) + offset);
	m_circle.setOrigin(v);
	m_rectangle.setOrigin(v);
	m_sprite.setOrigin(v);
	m_text.setOrigin(v);
}

void SFPrimitive::UpdateRenderState(Fixed alpha)
{
	if (!m_bWasDisabled && !m_renderState.bEnabled)
	{
		m_bWasDisabled = true;
		m_prevScale = Cast(m_renderState.sfScale.max);
		m_circle.setScale(ZERO);
		m_rectangle.setScale(ZERO);
		m_text.setString("");
		m_sprite.setScale(ZERO);
	}
	if (m_bWasDisabled && m_renderState.bEnabled)
	{
		m_bWasDisabled = false;
		m_circle.setScale(m_prevScale);
		m_rectangle.setScale(m_prevScale);
		m_text.setString(m_renderState.text);
		m_text.setCharacterSize(m_renderState.textSize);
		m_sprite.setScale(m_prevScale);
	}
	m_bRendered = true;
	if (m_bStatic || !m_renderState.bEnabled)
	{
		return;
	}

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
	
	if (m_renderState.bTextSet)
	{
		sf::String text = m_renderState.text;
		u32 textSize = m_renderState.textSize;
		m_text.setString(text);
		m_text.setCharacterSize(textSize);
	}
	
	UpdatePivot();

	sf::Vector2f scale = Cast(m_renderState.sfScale.Lerp(alpha));
	f32 orientation = Cast(m_renderState.sfOrientation.Lerp(alpha));
	sf::Vector2f position = Cast(m_renderState.sfPosition.Lerp(alpha));
	sf::Color fill =
		Cast(Colour::Lerp(m_renderState.sfPrimaryColour.min, m_renderState.sfPrimaryColour.max, alpha));
	sf::Color outline =
		Cast(Colour::Lerp(m_renderState.sfSecondaryColour.min, m_renderState.sfSecondaryColour.max, alpha));
	f32 outlineThickness = Cast(m_renderState.outlineThickness);
	
	m_circle.setScale(scale);
	m_circle.setRotation(orientation);
	m_circle.setPosition(position);
	m_rectangle.setScale(scale);
	m_rectangle.setRotation(orientation);
	m_rectangle.setPosition(position);
	m_sprite.setScale(scale);
	m_sprite.setRotation(orientation);
	m_sprite.setPosition(position);
	m_rectangle.setPosition(position);
	m_text.setScale(scale);
	m_text.setRotation(orientation);
	m_text.setPosition(position);

	m_circle.setFillColor(fill);
	m_circle.setOutlineThickness(outlineThickness);
	m_circle.setOutlineColor(outline);
	m_rectangle.setFillColor(fill);
	m_rectangle.setOutlineThickness(outlineThickness);
	m_rectangle.setOutlineColor(outline);
	m_text.setFillColor(fill);
	m_text.setOutlineThickness(outlineThickness);
	m_text.setOutlineColor(outline);
	m_sprite.setColor(fill);

	if (m_bMakeStatic)
	{
		m_bStatic = true;
		m_bMakeStatic = false;
	}
}

void SFPrimitive::Destroy()
{
	m_bDestroyed = true;
}
} // namespace LittleEngine
