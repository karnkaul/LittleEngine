#include "Transform.h"
#include "Utils.h"
#include <iostream>

namespace Core
{
Transform::Transform() = default;
Transform::~Transform()
{
	if (m_pParent)
	{
		m_pParent->RemoveChild(*this);
	}
	for (auto pChild : m_children)
	{
		if (pChild)
		{
			pChild->m_pParent = nullptr;
		}
	}
}

void Transform::SetParent(Transform& parent)
{
	if (m_pParent)
	{
		m_pParent->RemoveChild(*this);
	}
	m_pParent = &parent;
	parent.AddChild(*this);
	SetDirty();
}

void Transform::UnsetParent()
{
	if (m_pParent)
	{
		m_pParent->RemoveChild(*this);
	}
	m_pParent = nullptr;
	SetDirty();
}

Transform& Transform::SetPosition(Vector2 position)
{
	m_position = position;
	SetDirty();
	return *this;
}

Transform& Transform::SetOrientation(Vector2 orientation)
{
	m_orientation = orientation.Normalised();
	SetDirty();
	return *this;
}

Transform& Transform::SetOrientation(Fixed degrees)
{
	m_orientation = Vector2::ToOrientation(degrees);
	SetDirty();
	return *this;
}

Transform& Transform::SetScale(Vector2 scale)
{
	m_scale = scale;
	SetDirty();
	return *this;
}

Vector2 Transform::Position() const
{
	return m_position;
}

Vector2 Transform::Orientation() const
{
	return m_orientation;
}

Vector2 Transform::Scale() const
{
	return m_scale;
}

Vector2 Transform::WorldPosition() const
{
	WorldMatrix();
	return {m_mat.m_w.x, m_mat.m_w.y};
}

Vector2 Transform::WorldOrientation() const
{
	WorldMatrix();
	return {m_mat.m_x.x, m_mat.m_x.y};
}

Vector2 Transform::WorldScale() const
{
	Vector2 scale = m_scale;
	if (m_pParent)
	{
		Vector2 parent = m_pParent->WorldScale();
		scale.x *= parent.x;
		scale.y *= parent.y;
	}
	return scale;
}

const Matrix3& Transform::WorldMatrix(bool bForceRecalc) const
{
	if (m_bDirty || bForceRecalc)
	{
		m_mat = Matrix3(m_position, m_orientation, Vector2::One);
		if (m_pParent)
		{
			m_mat *= m_pParent->WorldMatrix();
		}
		m_bDirty = false;
	}
	return m_mat;
}

void Transform::AddChild(Transform& child)
{
	if (Core::Search(m_children, &child) == m_children.end())
	{
		m_children.push_back(&child);
	}
}

bool Transform::RemoveChild(Transform& child)
{
	return Core::Remove(m_children, &child);
}

void Transform::SetDirty()
{
	m_bDirty = true;
	for (auto pChild : m_children)
	{
		pChild->SetDirty();
	}
}
} // namespace Core
