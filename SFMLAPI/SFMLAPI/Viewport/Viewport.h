#pragma once
#include <memory>
#include "SFML/Graphics/RenderWindow.hpp"
#include "ViewportData.h"
#include "Core/CoreTypes.h"

namespace LittleEngine
{
// \brief: Derives from sf::RenderWindow
// Implementation provides SFML libs for linker via pragma
class Viewport final : public sf::RenderWindow
{
private:
	ViewportData m_data;
	Rect2 m_viewBounds;
	Vector2 m_viewSize;

public:
	static ViewportSize GetMaxSize();

public:
	Viewport();
	~Viewport() override;

	void SetData(ViewportData data);
	void Create();
	void Destroy();
	void OverrideData(ViewportRecreateData data);

	Vector2 GetViewSize() const;
	// Projects unit Rect to screen
	Vector2 Project(Vector2 nPos, bool bPreClamp) const;
	Vector2 ScreenToWorld(s32 screenX, s32 screenY) const;
};
} // namespace LittleEngine
