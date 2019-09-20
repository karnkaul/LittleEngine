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

public:
	static ViewportSize MaxSize();

public:
	Viewport();
	~Viewport() override;

	void SetData(ViewportData data);
	void Create(u32 framerateLimit = 300);
	void Destroy();
	void OverrideData(ViewportRecreateData data);

	Vector2 ViewportToWorld(s32 vpX, s32 vpY) const;
};
} // namespace LittleEngine
