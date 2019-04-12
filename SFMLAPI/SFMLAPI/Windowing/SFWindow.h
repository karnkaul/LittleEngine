#pragma once
#include <memory>
#include "SFML/Graphics.hpp"
#include "SFWindowData.h"
#include "CoreTypes.h"

namespace LittleEngine
{
// \brief: Derives from sf::RenderWindow
// Implementation provides SFML libs for linker via pragma
class SFWindow final : public sf::RenderWindow
{
private:
	SFWindowData m_data;
	Rect2 m_viewBounds;

public:
	static SFWindowSize GetMaxWindowSize();

public:
	SFWindow();
	~SFWindow() override;

	void SetData(SFWindowData data);
	void CreateWindow();
	void DestroyWindow();
	void OverrideData(SFWindowRecreateData data);

	Vector2 GetViewSize() const;
	// Projects unit Rect to screen
	Vector2 Project(Vector2 nPos, bool bPreClamp) const;
};
} // namespace LittleEngine
