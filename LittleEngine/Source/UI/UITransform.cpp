#include "le_stdafx.h"
#include "Utils.h"
#include "UITransform.h"
#include "SFMLInterface/Graphics.h"

namespace LittleEngine {
	UITransform::UITransform() : size(Graphics::GetWorldRect().GetSize()) {}
	UITransform::UITransform(const Vector2& size, const Vector2& nPosition, const Vector2& anchor, const Vector2& pixelPad) : size(size), nPosition(nPosition), anchor(anchor), pixelPad(pixelPad) {}

	UITransform::~UITransform() {
		if (pParent) pParent->RemoveChild(*this);
		for (auto child : children) {
			if (child) child->pParent = nullptr;
		}
	}

	void UITransform::SetParent(UITransform & parent) {
		pParent = &parent;
		parent.AddChild(*this);
	}

	void UITransform::UnsetParent() {
		if (pParent) pParent->RemoveChild(*this);
		pParent = nullptr;
	}

	Vector2 UITransform::GetWorldPosition() {
		Vector2 offset = pParent ? pParent->GetWorldPosition() : Vector2::Zero;
		nPosition.x = Maths::Clamp_11(nPosition.x);
		nPosition.y = Maths::Clamp_11(nPosition.y);
		Vector2 scale = pParent ? Fixed::OneHalf * pParent->size : Graphics::GetWorldRect().upper;
		return Vector2(nPosition.x * scale.x, nPosition.y * scale.y) + offset + pixelPad;
	}

	void UITransform::AddChild(UITransform & child) {
		children.push_back(&child);
	}

	bool UITransform::RemoveChild(UITransform & child) {
		return GameUtils::VectorErase(children, &child);
	}
}
