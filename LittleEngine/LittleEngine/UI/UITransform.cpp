#include "stdafx.h"
#include "Core/Utils.h"
#include "UITransform.h"
#include "LittleEngine/GFX/GFX.h"

namespace LittleEngine
{
UITransform::UITransform() : size(GFX::GetViewSize())
{
}
UITransform::UITransform(Vector2 size, Vector2 nPosition, Vector2 anchor, Vector2 pixelPad)
	: size(size), nPosition(nPosition), anchor(anchor), padding(pixelPad)
{
}

UITransform::~UITransform()
{
	if (pParent)
	{
		pParent->RemoveChild(*this);
	}
	for (auto child : children)
	{
		if (child)
		{
			child->pParent = nullptr;
		}
	}
}

void UITransform::SetParent(UITransform& parent)
{
	pParent = &parent;
	parent.AddChild(*this);
}

void UITransform::UnsetParent()
{
	if (pParent)
	{
		pParent->RemoveChild(*this);
	}
	pParent = nullptr;
}

void UITransform::SetAutoPadNPosition(Vector2 nPosition, bool bClamp)
{
	padding = -Fixed::OneHalf * size;
	if (bClamp)
	{
		this->nPosition.x = Maths::Clamp_11(nPosition.x);
		this->nPosition.y = Maths::Clamp_11(nPosition.y);
	}
	else
	{
		this->nPosition = nPosition;
	}
	padding.x *= this->nPosition.x;
	padding.y *= this->nPosition.y;
}

Vector2 UITransform::GetWorldPosition() const
{
	Vector2 offset = pParent ? pParent->GetWorldPosition() : Vector2::Zero;
	Vector2 scale = pParent ? Fixed::OneHalf * pParent->size : Fixed::OneHalf * GFX::GetViewSize();
	return Vector2(nPosition.x * scale.x, nPosition.y * scale.y) + offset + padding;
}

void UITransform::AddChild(UITransform& child)
{
	children.push_back(&child);
}

bool UITransform::RemoveChild(UITransform& child)
{
	return Core::Remove(children, &child);
}
} // namespace LittleEngine
