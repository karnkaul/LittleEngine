#pragma once
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
	Vec<Transform*> m_children;
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

	Vector2 GetPosition() const;
	Vector2 GetOrientation() const;
	Vector2 GetScale() const;

	Vector2 GetWorldPosition() const;
	Vector2 GetWorldOrientation() const;
	Vector2 GetWorldScale() const;

	const Matrix3& GetWorldMatrix(bool bForceRecalc = false) const;

private:
	void AddChild(Transform& child);
	bool RemoveChild(Transform& child);
	void SetDirty();
};
} // namespace Core
