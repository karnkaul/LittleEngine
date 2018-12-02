#pragma once
#include <vector>
#include "Vector2.h"

namespace GameUtils {
	// \brief Class maintains a position and orientation in 2D space, 
	// and provides an API for parenting to other Transforms.
	struct Transform {
	public:
		const static Transform IDENTITY;

		// Position world space
		Vector2 localPosition = Vector2::Zero;
		// Rotation in world orientation (+ is clockwise)
		Fixed localOrientation = Fixed::Zero;
		Vector2 localScale = Vector2::One;

		Transform();
		~Transform();

		// Attaches this to another transform as its parent
		void SetParent(Transform& parent, bool bModifyWorldSpace = true);
		// Unsets parent, if any
		void UnsetParent(bool bModifyWorldPosition = true);

		// Note: Might return nullptr
		Transform* GetParent() const;
		// Returns position from parent's origin as position in world space
		Vector2 Position() const;
		// Returns rotation in world orientation (+ is clockwise)
		Fixed Orientation() const;
		// Returns parent-aware scale
		Vector2 Scale() const;
		// Call this to Rotate this and all children
		void Rotate(Fixed angle);

		std::string ToString() const;

	private:
		std::vector<Transform*> m_children;
		Transform* m_parent = nullptr;

		void AddChild(Transform* child);
		void RemoveChild(Transform* child);
	};
}
