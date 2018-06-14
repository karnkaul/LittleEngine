#pragma once
#include <vector>
#include "Utils/Vector2.h"

namespace Consts {
	const Fixed PI = Fixed(3.14159265359);
	const Fixed DEG_TO_RAD = Fixed(PI / 180);
}

class Transform : public std::enable_shared_from_this<Transform> {
public:
	// Position world space
	Vector2 localPosition;
	// Rotation in world orientation (+ is clockwise)
	Fixed localRotation;
	Transform() : localPosition(Vector2::Zero()), localRotation(Fixed(0)) {}
	
	// Call this to attach this to another transform as its parent
	void SetParent(std::weak_ptr<Transform> parent, bool modifyWorldSpace = true);
	std::weak_ptr<Transform> GetParent() const;
	// Returns position from parent's origin as position in world space
	Vector2 Position() const;
	// Returns rotation in world orientation (+ is clockwise)
	Fixed Rotation();
	// Call this to Rotate this and all children
	void Rotate(Fixed angle);
	std::string ToString() const;

private:
	std::weak_ptr<Transform> m_parent;
	void AddChild(std::shared_ptr<Transform> child);
	std::vector<std::weak_ptr<Transform> > m_children;
};
