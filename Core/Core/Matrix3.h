#pragma once
#include "Vector2.h"

namespace Core
{
struct Vector3
{
	Fixed x;
	Fixed y;
	Fixed z;

	Fixed SqrMagnitude() const;
	Fixed Magnitude() const;

	void Normalise();
	Vector3 Normalised() const;

	inline void Set(Vector2 xy)
	{
		x = xy.x;
		y = xy.y;
	}

	inline void Set(Vector2 xy, Fixed z)
	{
		Set(xy);
		this->z = z;
	}
};

// Row-major
class Matrix3
{
public:
	Vector3 m_x; // [1, 0, 0]
	Vector3 m_y; // [0, 1, 0]
	Vector3 m_w; // [0, 0, 1]

public:
	static const Matrix3 ZERO;

public:
	Matrix3();
	Matrix3(Vector3 x, Vector3 y, Vector3 w);
	Matrix3(Vector2 position, Vector2 orientation, Vector2 scale);

	Matrix3& operator*=(const Matrix3& rhs);
	void Normalise();
};

Matrix3 operator*(const Matrix3& lhs, const Matrix3& rhs);
Vector2 operator*(Vector2 lhs, const Matrix3& rhs);

class MatTransform
{
private:
	mutable Matrix3 m_mat;
	Vector2 m_position;
	Vector2 m_orientation = Vector2::Up;
	Vector2 m_scale = Vector2::One;
	mutable bool m_bDirty = false;

public:
	MatTransform& SetPosition(Vector2 position);
	MatTransform& SetOrientation(Vector2 orientation);
	MatTransform& SetOrientation(Fixed degrees);
	MatTransform& SetScale(Vector2 scale);

	Vector2 GetPosition() const;
	Vector2 GetOrientation() const;
	Vector2 GetScale() const;

	const Matrix3& GetWorldMatrix(bool bForceRecalc = false) const;
};
} // namespace Core
