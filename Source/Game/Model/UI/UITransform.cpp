#include "Core/Utils.h"
#include "UITransform.h"

namespace LittleEngine
{
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
	if (pParent)
	{
		pParent->RemoveChild(*this);
	}
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

Vector2 UITransform::WorldPosition(Vector2 worldSize) const
{
	Vector2 offset = pParent ? pParent->WorldPosition(worldSize) : Vector2::Zero;
	Vector2 scale = pParent ? Fixed::OneHalf * pParent->size : Fixed::OneHalf * worldSize;
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
