#include "stdafx.h"
#include "SFPrimitive.h"
#include "Logger.h"

namespace LittleEngine
{
Vector2 SFPrimitive::WorldToScreen(const Vector2& worldPoint)
{
	return Vector2(worldPoint.x, -worldPoint.y);
}

Fixed SFPrimitive::WorldToScreen(const Fixed& worldOrientation)
{
	return -worldOrientation;
}

Vector2 SFPrimitive::ScreenToWorld(const Vector2& screenPoint)
{
	return Vector2(screenPoint.x, -screenPoint.y);
}

Fixed SFPrimitive::ScreenToWorld(const Fixed& screenOrientation)
{
	return -screenOrientation;
}

SFPrimitive::SFPrimitive()
{
	SetPivot(Vector2::Zero);
}

SFPrimitive::SFPrimitive(const SFPrimitiveData& data)
{
	SetPrimaryColour(data.primary);
	SetSecondaryColour(data.secondary);
	if (data.texture)
		SetTexture(*data.texture);
	if (data.font)
		SetFont(*data.font);
	if (!data.text.empty())
	{
		SetText(data.text);
		SetTextSize(DEFAULT_TEXT_SIZE);
	}
	SetPivot(Vector2::Zero);
}

SFPrimitive* SFPrimitive::SetEnabled(bool bEnabled)
{
	m_state.bEnabled = bEnabled;
	return this;
}

SFPrimitive* SFPrimitive::SetLayer(LayerID layer)
{
	m_state.layer = layer;
	return this;
}

SFPrimitive* SFPrimitive::SetPosition(const Vector2& sfPosition, bool bImmediate)
{
	if (bImmediate)
		m_state.sfPosition.Reset(WorldToScreen(sfPosition));
	else
		m_state.sfPosition.Update(WorldToScreen(sfPosition));
	return this;
}

SFPrimitive* SFPrimitive::SetOrientation(const Fixed& sfOrientation, bool bImmediate)
{
	if (bImmediate)
		m_state.sfOrientation.Reset(WorldToScreen(sfOrientation));
	else
		m_state.sfOrientation.Update(WorldToScreen(sfOrientation));
	return this;
}

SFPrimitive* SFPrimitive::SetScale(const Vector2& sfScale, bool bImmediate)
{
	if (bImmediate)
		m_state.sfScale.Reset(sfScale);
	else
		m_state.sfScale.Update(sfScale);
	return this;
}

SFPrimitive* SFPrimitive::SetPivot(const Vector2& pivot)
{
	m_state.pivot = pivot;
	return this;
}

SFPrimitive* SFPrimitive::SetPrimaryColour(Colour sfColour, bool bImmediate)
{
	if (bImmediate)
		m_state.sfPrimaryColour.Reset(sfColour);
	else
		m_state.sfPrimaryColour.Update(sfColour);
	return this;
}

SFPrimitive* SFPrimitive::SetSecondaryColour(Colour sfColour, bool bImmediate)
{
	if (bImmediate)
		m_state.sfSecondaryColour.Reset(sfColour);
	else
		m_state.sfSecondaryColour.Update(sfColour);
	return this;
}

SFPrimitive* SFPrimitive::SetOutline(const Fixed& thickness)
{
	m_state.outlineThickness = thickness;
	return this;
}

SFPrimitive* SFPrimitive::SetSize(const Vector2& size, SFShapeType onShape)
{
	sf::Vector2f s = Cast(size);
	if (onShape == SFShapeType::Circle)
		m_circle.setRadius(s.x);
	else
		m_rectangle.setSize(s);
	return this;
}

SFPrimitive* SFPrimitive::SetTexture(const TextureAsset& texture)
{
	m_sprite.setTexture(texture.m_sfTexture);
	return this;
}

SFPrimitive* SFPrimitive::Crop(const Rect2& rect)
{
	sf::IntRect textureRect(rect.GetTopLeft().x.ToS32(), rect.GetTopLeft().y.ToS32(),
							rect.GetSize().x.ToS32(), rect.GetSize().y.ToS32());
	m_sprite.setTextureRect(textureRect);
	return this;
}

SFPrimitive* SFPrimitive::SetFont(const FontAsset& font)
{
	m_text.setFont(font.m_sfFont);
	return this;
}

SFPrimitive* SFPrimitive::SetTextSize(u32 pixelSize)
{
	m_text.setCharacterSize(pixelSize);
	return this;
}

SFPrimitive* SFPrimitive::SetText(const String& text)
{
	m_text.setString(text);
	return this;
}

Rect2 SFPrimitive::GetBounds() const
{
	Rect2 ret = GetShapeBounds();
	Rect2 temp = GetSpriteBounds();
	if (temp.GetSize().SqrMagnitude() > ret.GetSize().SqrMagnitude())
		ret = temp;
	temp = GetTextBounds();
	if (temp.GetSize().SqrMagnitude() > ret.GetSize().SqrMagnitude())
		ret = temp;
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
	return m_state.bEnabled;
}

Vector2 SFPrimitive::GetPosition() const
{
	return ScreenToWorld(m_state.sfPosition.max);
}

Fixed SFPrimitive::GetOrientation() const
{
	return ScreenToWorld(m_state.sfOrientation.max);
}

Vector2 SFPrimitive::GetScale() const
{
	return m_state.sfScale.max;
}

Colour SFPrimitive::GetPrimaryColour() const
{
	return m_state.sfPrimaryColour.max;
}

Colour SFPrimitive::GetSecondaryColour() const
{
	return m_state.sfSecondaryColour.max;
}

LayerID SFPrimitive::GetLayer() const
{
	return m_state.layer;
}

void SFPrimitive::ReconcileState()
{
	m_state.Reconcile();
}

void SFPrimitive::UpdatePivot()
{
	Vector2 offset = Fixed::OneHalf * GetBounds().GetSize();
	Vector2 pivot = Vector2(offset.x * m_state.pivot.x, offset.y * m_state.pivot.y);
	sf::Vector2f v = Cast(WorldToScreen(pivot) + offset);
	m_circle.setOrigin(v);
	m_rectangle.setOrigin(v);
	m_sprite.setOrigin(v);
	m_text.setOrigin(v);
}

void SFPrimitive::UpdateRenderState(const Fixed& alpha)
{
	if (!m_state.bEnabled)
		LogE("DISABLED PRIMITIVE BEING UPDATED");
	UpdatePivot();
	sf::Vector2f scale = Cast(m_state.sfScale.Lerp(alpha));
	f32 orientation = Cast(m_state.sfOrientation.Lerp(alpha));
	sf::Vector2f position = Cast(m_state.sfPosition.Lerp(alpha));
	sf::Color fill = Cast(Colour::Lerp(m_state.sfPrimaryColour.min, m_state.sfPrimaryColour.max, alpha));
	sf::Color outline =
		Cast(Colour::Lerp(m_state.sfSecondaryColour.min, m_state.sfSecondaryColour.max, alpha));
	f32 outlineThickness = Cast(m_state.outlineThickness);

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

#if DEBUGGING
	if (bDebugThisPrimitive)
	{
		/*Colour c = Colour::Lerp(m_state.sfSecondaryColour.min, m_state.sfSecondaryColour.max, alpha);
		sf::Color sfC = Cast(c);
		String sfCText = Strings::ToString(sfC.r) + ", " + Strings::ToString(sfC.g) + ", " + Strings::ToString(sfC.b) + ", " + Strings::ToString(sfC.a);
		LogD("Colour: " + c.ToString() + " sfColor: " + sfCText + " outline: " + Strings::ToString(outlineThickness));*/
		LogW("Enabled: " + Strings::ToString(m_state.bEnabled));
	}
#endif
}
} // namespace LittleEngine
