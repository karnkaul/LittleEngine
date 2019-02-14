#pragma once
#include <memory>
#include "CoreTypes.h"
#include "SFML/Graphics.hpp"
#include "SFWindowData.h"

namespace LittleEngine
{
// \brief: Derives from sf::RenderWindow
// Implementation provides SFML libs for linker via pragma
class SFWindow final : public sf::RenderWindow
{
private:
	Rect2 m_viewBounds;

public:
	SFWindow(const SFWindowData& windowData);

	Vector2 GetViewSize() const;
	// Projects unit Rect to screen
	Vector2 Project(const Vector2& nPos, bool bPreClamp) const;
};
} // namespace LittleEngine
