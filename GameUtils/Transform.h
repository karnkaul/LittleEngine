#pragma once
#include <vector>
#include "Vector2.h"

namespace GameUtils {
	// \brief Class maintains a position and orientation in 2D space, 
	// and provides an API for parenting to other Transforms.
	struct Transform {
	public:
		static const Transform IDENTITY;

	private:
		std::vector<Transform*> children;
		Transform* pParent = nullptr;
	public:
		Vector2 localPosition;		// screen centre is origin
		Vector2 localScale;
		Fixed localOrientation;		// + is clockwise

	public:
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
		void AddChild(Transform* pChild);
		void RemoveChild(Transform* pChild);
	};
}
