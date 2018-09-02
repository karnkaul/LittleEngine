#include "stdafx.h"
#include <iostream>
#include "Transform.h"
#include "Utils/Utils.h"

void Transform::SetParent(Transform& parent, bool modifyWorldSpace) {
	m_parent = &parent;
	if (!modifyWorldSpace) {
		localPosition -= parent.localPosition;
		localRotation -= parent.localRotation;
	}
	parent.AddChild(this);
}

void Transform::UnsetParent(bool modifyWorldPosition) {
	if (m_parent != nullptr) {
		if (!modifyWorldPosition) {
			localPosition = Position();
		}
		m_parent->RemoveChild(this);
	}
	m_parent = nullptr;
}

Transform* Transform::GetParent() const {
	return m_parent;
}

Vector2 Transform::Position() const {
	Vector2 position = localPosition;
	if (m_parent != nullptr) {
		position += m_parent->Position();
	}
	return position;
}

Fixed Transform::Rotation() {
	localRotation %= 360;
	Fixed rotation = localRotation;
	if (m_parent != nullptr) {
		rotation += m_parent->Rotation();
	}
	return rotation;
}

void Transform::Rotate(Fixed angle) {
	localRotation += angle;
	Utils::CleanVector<Transform*>(m_children, [](Transform* child) { return child == nullptr; });
	// Children need to be repositioned
	if (!m_children.empty()) {
		Fixed rad = angle * Consts::DEG_TO_RAD;
		Fixed s = rad.Sin(), c = rad.Cos();
		for (const auto& child : m_children) {
			//if (Ptr transform = child.lock()) {
			if (child != nullptr) {
				Vector2 p = child->localPosition;
				child->localPosition = Vector2(
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

Transform::~Transform() {
	if (m_parent != nullptr) {
		m_parent->RemoveChild(this);
	}
	if (!m_children.empty()) {
		for (auto child : m_children) {
			if (child != nullptr) {
				child->UnsetParent();
			}
		}
	}
}

void Transform::AddChild(Transform* child) {
	m_children.emplace_back(child);
}

void Transform::RemoveChild(Transform* child) {
	if (child != nullptr) {
		auto search = Utils::Find<Transform*>(m_children, child);
		if (search != m_children.end()) {
			m_children.erase(search);
		}
	}
}
