#include "stdafx.h"
#include <iostream>
#include "Transform.h"

Transform::Ptr Transform::Create() {
	struct shared_enabler : public Transform {};
	return std::make_shared<shared_enabler>();
}

void Transform::SetParent(Ptr parent, bool modifyWorldSpace) {
	m_parent = parent;
	if (!modifyWorldSpace) {
		localPosition -= parent->localPosition;
		localRotation -= parent->localRotation;
	}
	parent->AddChild(this->shared_from_this());
}

Transform::wPtr Transform::GetParent() const {
	return m_parent;
}

Vector2 Transform::Position() const {
	Vector2 position = localPosition;
	Ptr transform;
	if (transform = m_parent.lock()) {
		position += transform->Position();
	}
	return position;
}

Fixed Transform::Rotation() {
	localRotation %= 360;
	Fixed rotation = localRotation;
	Ptr transform;
	if (transform = m_parent.lock()) {
		rotation += transform->Rotation();
	}
	return rotation;
}

void Transform::Rotate(Fixed angle) {
	localRotation += angle;
	// Children need to be repositioned
	if (!m_children.empty()) {
		Fixed rad = angle * Consts::DEG_TO_RAD;
		for (const auto& child : m_children) {
			Ptr transform;
			if (transform = child.lock()) {
				Vector2 p = transform->localPosition;
				Fixed s = rad.Sin(), c = rad.Cos();
				transform->localPosition = Vector2(
					(p.x * c) - (p.y * s),
					(p.x * s) + (p.y * c)
				);
			}
		}
	}
}

std::string Transform::ToString() const {
	return localPosition.ToString() + " , " + localRotation.ToString();
}

Transform::Transform() : localPosition(Vector2::Zero), localRotation(Fixed::Zero) {}

Transform::~Transform() = default;

void Transform::AddChild(Ptr child) {
	m_children.emplace_back(child);
}
