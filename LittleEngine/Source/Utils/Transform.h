#pragma once
#include <vector>
#include "Utils/Vector2.h"

namespace Consts {
	const Fixed PI = Fixed(3.14159265359);
	const Fixed DEG_TO_RAD = Fixed(PI / 180);
}

// \brief Class maintains a position and orientation in 2D space, 
// and provides an API for parenting to other Transforms.
// Must be created using shared_ptr<Transform>
class Transform : public std::enable_shared_from_this<Transform> {
public:
	using Ptr = std::shared_ptr<Transform>;
	using wPtr = std::weak_ptr<Transform>;

	// Position world space
	Vector2 localPosition;
	// Rotation in world orientation (+ is clockwise)
	Fixed localRotation;
	
	static Ptr Create();
	Transform& operator=(const Transform&) = delete;
	Transform(Transform&&) = delete;
	~Transform();
	
	// Call this to attach this to another transform as its parent
	void SetParent(Transform& parent, bool modifyWorldSpace = true);
	wPtr GetParent() const;
	// Returns position from parent's origin as position in world space
	Vector2 Position() const;
	// Returns rotation in world orientation (+ is clockwise)
	Fixed Rotation();
	// Call this to Rotate this and all children
	void Rotate(Fixed angle);

	std::string ToString() const;

private:
	std::vector<wPtr> m_children;
	wPtr m_parent;

	Transform();
	
	void AddChild(Ptr child);
	
};
