#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
// \brief Screen Rect based Transform for UIElements
struct UITransform
{
public:
	Vector2 nPosition = Vector2::Zero;
	Vector2 anchor = Vector2::Zero;
	Vector2 posDelta;

private:
	Vector2 size;
	Vec<UITransform*> children;
	UITransform* pParent = nullptr;
	bool bAutoPad = false;

public:
	~UITransform();

	void SetParent(UITransform& parent);
	void UnsetParent();

	// Returns anchor's position in world space
	Vector2 WorldPosition(Vector2 worldSize) const;

private:
	void SetAutoPadNPosition(Vector2 nPosition, bool bClamp = true);

	void AddChild(UITransform& child);
	bool RemoveChild(UITransform& child);

	friend class UIElement;
};
} // namespace LittleEngine
