#pragma once
#include <memory>
#include "CoreTypes.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine
{
// \brief: Derives from sf::RenderWindow
// Implementation provides SFML libs for linker via pragma
class SFWindow final : public sf::RenderWindow
{
private:
	Rect2 m_viewBounds;

public:
	static struct SFWindowSize GetMaxWindowSize();

public:
	SFWindow(const struct SFWindowData& windowData);
	~SFWindow();

	Vector2 GetViewSize() const;
	// Projects unit Rect to screen
	Vector2 Project(const Vector2& nPos, bool bPreClamp) const;

	void SetSize(const struct SFWindowSize& size);
};
} // namespace LittleEngine
