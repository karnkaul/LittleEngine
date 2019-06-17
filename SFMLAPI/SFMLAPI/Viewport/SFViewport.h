#pragma once
#include <memory>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFViewportData.h"
#include "Core/CoreTypes.h"

namespace LittleEngine
{
// \brief: Derives from sf::RenderWindow
// Implementation provides SFML libs for linker via pragma
class SFViewport final : public sf::RenderWindow
{
private:
	SFViewportData m_data;
	Rect2 m_viewBounds;
	Vector2 m_viewSize;

public:
	static SFViewportSize GetMaxSize();

public:
	SFViewport();
	~SFViewport() override;

	void SetData(SFViewportData data);
	void Create();
	void Destroy();
	void OverrideData(SFViewportRecreateData data);

	Vector2 GetViewSize() const;
	// Projects unit Rect to screen
	Vector2 Project(Vector2 nPos, bool bPreClamp) const;
	Vector2 ScreenToWorld(s32 screenX, s32 screenY) const;
};
} // namespace LittleEngine
