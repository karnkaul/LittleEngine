#pragma once
#include <vector>
#include "Matrix3.h"

namespace Core
{
class Transform final
{
private:
	mutable Matrix3 m_mat;
	Vector2 m_position;
	Vector2 m_orientation = Vector2::Right;
	Vector2 m_scale = Vector2::One;
	std::vector<Transform*> m_children;
	Transform* m_pParent = nullptr;
	mutable bool m_bDirty = false;

public:
	Transform();
	~Transform();

	void SetParent(Transform& parent);
	void UnsetParent();

	Transform& SetPosition(Vector2 position);
	Transform& SetOrientation(Vector2 orientation);
	Transform& SetOrientation(Fixed degrees);
	Transform& SetScale(Vector2 scale);

	Vector2 Position() const;
	Vector2 Orientation() const;
	Vector2 Scale() const;

	Vector2 WorldPosition() const;
	Vector2 WorldOrientation() const;
	Vector2 WorldScale() const;

	const Matrix3& WorldMatrix(bool bForceRecalc = false) const;

private:
	void AddChild(Transform& child);
	bool RemoveChild(Transform& child);
	void SetDirty();
};
} // namespace Core
