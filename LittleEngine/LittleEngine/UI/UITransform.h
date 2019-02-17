#pragma once
#include "CoreTypes.h"

namespace LittleEngine
{
// \brief Screen Rect based Transform for UIElements
struct UITransform
{
public:
	Vector2 size;
	Vector2 nPosition = Vector2::Zero;
	Vector2 anchor = Vector2::Zero;
	Vector2 padding;

private:
	Vector<UITransform*> children;
	UITransform* pParent = nullptr;

public:
	UITransform();
	UITransform(const Vector2& size,
				const Vector2& nPosition = Vector2::Zero,
				const Vector2& anchor = Vector2::Zero,
				const Vector2& pixelPad = Vector2(2, 2));
	~UITransform();

	void SetParent(UITransform& parent);
	void UnsetParent();

	void SetAutoPadNPosition(const Vector2& nPosition, bool bClamp = true);
	// Returns anchor's position in world space
	Vector2 GetWorldPosition() const;

private:
	void AddChild(UITransform& child);
	bool RemoveChild(UITransform& child);
};
} // namespace LittleEngine