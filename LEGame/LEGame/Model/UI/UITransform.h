#pragma once
#include "Core/CoreTypes.h"

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
	bool bAutoPad = false;

private:
	Vec<UITransform*> children;
	UITransform* pParent = nullptr;

public:
	UITransform(class LEContext* pContext = nullptr);
	UITransform(Vector2 size,
				Vector2 nPosition = Vector2::Zero,
				Vector2 anchor = Vector2::Zero,
				Vector2 pixelPad = Vector2(2, 2));
	~UITransform();

	void SetParent(UITransform& parent);
	void UnsetParent();

	// Returns anchor's position in world space
	Vector2 GetWorldPosition(Vector2 viewSize) const;

private:
	void SetAutoPadNPosition(Vector2 nPosition, bool bClamp = true);

	void AddChild(UITransform& child);
	bool RemoveChild(UITransform& child);

	friend class UIElement;
};
} // namespace LittleEngine
