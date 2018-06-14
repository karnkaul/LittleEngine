#include "stdafx.h"
#include <iostream>
#include "Transform.h"

void Transform::SetParent(std::weak_ptr<Transform> parent, bool modifyWorldSpace) {
	m_parent = parent;
	std::shared_ptr<Transform> transform;
	if (transform = m_parent.lock()) {
		if (!modifyWorldSpace) {
			localPosition -= transform->localPosition;
			localRotation -= transform->localRotation;
		}
		transform->AddChild(this->shared_from_this());
	}
}

std::weak_ptr<Transform> Transform::GetParent() const {
	return m_parent;
}

Vector2 Transform::Position() const {
	Vector2 position = localPosition;
	std::shared_ptr<Transform> transform;
	if (transform = m_parent.lock()) {
		position += transform->Position();
	}
	return position;
}

Fixed Transform::Rotation() {
	localRotation %= 360;
	Fixed rotation = localRotation;
	std::shared_ptr<Transform> transform;
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
			std::shared_ptr<Transform> transform;
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

void Transform::AddChild(std::shared_ptr<Transform> child) {
	m_children.emplace_back(child);
}
