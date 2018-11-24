#include <iostream>
#include "Transform.h"
#include "Utils.h"

namespace GameUtils {
	const Transform Transform::IDENTITY = Transform();

	void Transform::SetParent(Transform& parent, bool bModifyWorldSpace) {
		m_parent = &parent;
		if (!bModifyWorldSpace) {
			localPosition -= parent.localPosition;
			localOrientation -= parent.localOrientation;
		}
		parent.AddChild(this);
	}

	void Transform::UnsetParent(bool bModifyWorldPosition) {
		if (m_parent != nullptr) {
			if (!bModifyWorldPosition) {
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
		return std::move(position);
	}

	Fixed Transform::Orientation() const {
		Fixed rotation = localOrientation % Fixed(360);
		if (m_parent != nullptr) {
			rotation += m_parent->Orientation();
		}
		return std::move(rotation);
	}

	Vector2 Transform::Scale() const {
		Vector2 ret = localScale;
		if (m_parent) {
			ret += m_parent->Scale();
		}
		return ret;
	}

	void Transform::Rotate(Fixed angle) {
		localOrientation += angle;
		GameUtils::CleanVector<Transform*>(m_children, [](Transform* child) { return child == nullptr; });
		// Children need to be repositioned
		if (!m_children.empty()) {
			Fixed rad = angle * Maths::DEG_TO_RAD;
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
		return localPosition.ToString() + " , " + localOrientation.ToString();
	}

	Transform::Transform() : localPosition(Vector2::Zero), localOrientation(Fixed::Zero) {}

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
			GameUtils::VectorErase(m_children, child);
		}
	}
}
