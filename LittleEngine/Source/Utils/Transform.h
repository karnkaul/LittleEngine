#pragma once
#include <vector>
#include "Utils/Vector2.h"

namespace Consts {
	const Fixed PI = Fixed(3.14159265359);
	const Fixed DEG_TO_RAD = Fixed(PI / 180);
}

class Transform : public std::enable_shared_from_this<Transform> {
public:
	using Ptr = std::shared_ptr<Transform>;
	using wPtr = std::weak_ptr<Transform>;
	
	static Ptr Create();
	~Transform();

	// Position world space
	Vector2 localPosition;
	// Rotation in world orientation (+ is clockwise)
	Fixed localRotation;
	
	// Call this to attach this to another transform as its parent
	void SetParent(Ptr parent, bool modifyWorldSpace = true);
	wPtr GetParent() const;
	// Returns position from parent's origin as position in world space
	Vector2 Position() const;
	// Returns rotation in world orientation (+ is clockwise)
	Fixed Rotation();
	// Call this to Rotate this and all children
	void Rotate(Fixed angle);
	std::string ToString() const;

private:
	Transform();
	wPtr m_parent;
	void AddChild(Ptr child);
	std::vector<wPtr> m_children;
};
