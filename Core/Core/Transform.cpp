#include "stdafx.h"
#include "Transform.h"
#include "Utils.h"
#include <iostream>

namespace Core
{
const Transform Transform::IDENTITY = Transform();

Transform::Transform()
	: localPosition(Vector2::Zero), localScale(Vector2::One), localOrientation(Fixed::Zero)
{
}

Transform::~Transform()
{
	if (pParent)
	{
		pParent->RemoveChild(*this);
	}
	for (auto pChild : pChildren)
	{
		if (pChild)
		{
			pChild->pParent = nullptr;
		}
	}
}

void Transform::SetParent(Transform& parent, bool bModifyWorldSpace)
{
	pParent = &parent;
	if (!bModifyWorldSpace)
	{
		localPosition -= parent.localPosition;
		localOrientation -= parent.localOrientation;
	}
	parent.AddChild(*this);
}

void Transform::UnsetParent(bool bModifyWorldSpace)
{
	if (pParent)
	{
		if (!bModifyWorldSpace)
		{
			localPosition = Position();
			localOrientation = Orientation();
		}

		pParent->RemoveChild(*this);
	}
	pParent = nullptr;
}

Transform* Transform::GetParent() const
{
	return pParent;
}

Vector2 Transform::Position() const
{
	Vector2 position = localPosition;
	if (pParent)
	{
		position += pParent->Position();
	}
	return position;
}

Fixed Transform::Orientation() const
{
	Fixed orientation = localOrientation;
	if (pParent)
	{
		orientation += pParent->Orientation();
	}
	return orientation;
}

Vector2 Transform::Scale() const
{
	Vector2 ret = localScale;
	if (pParent)
	{
		ret.x *= pParent->Scale().x;
		ret.y *= pParent->Scale().y;
	}
	return ret;
}

void Transform::Rotate(Fixed angle)
{
	localOrientation += angle;
	Core::RemoveIf<Transform*>(pChildren, [](Transform* child) { return child == nullptr; });
	// Children need to be repositioned
	if (!pChildren.empty())
	{
		Fixed rad = angle * Maths::DEG_TO_RAD;
		Fixed s = rad.Sin();
		Fixed c = rad.Cos();
		for (auto pChild : pChildren)
		{
			if (pChild)
			{
				Vector2 p = pChild->localPosition;
				pChild->localPosition = Vector2((p.x * c) - (p.y * s), (p.x * s) + (p.y * c));
			}
		}
	}
}

String Transform::ToString() const
{
	return localPosition.ToString() + " , " + localOrientation.ToString();
}

void Transform::AddChild(Transform& child)
{
	pChildren.push_back(&child);
}

bool Transform::RemoveChild(Transform& child)
{
	return Core::Remove(pChildren, &child);
}
} // namespace Core
