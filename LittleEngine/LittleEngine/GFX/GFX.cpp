#include "stdafx.h"
#include "GFX.h"
#include "LittleEngine/Game/GameSettings.h"
#include "SFMLAPI/Windowing/SFWindow.h"

namespace LittleEngine
{
namespace
{
const SFWindow* pSFWindow = nullptr;
Map<u32, SFWindowSize> windowSizes;
}

void GFX::Init(const SFWindow& sfWindow)
{
	pSFWindow = &sfWindow;
	
	SFWindowSize max = SFWindow::GetMaxWindowSize();
	Vec<u32> heights = {360, 540, 720, 900, 1080, 1440, 2160};
	windowSizes.clear();
	sf::Vector2u w = sfWindow.getSize();
	for (auto height : heights)
	{
		u32 width = (w.x * height) / w.y;
		if (width <= max.width && height <= max.height)
		{
			windowSizes.emplace(height, SFWindowSize(width, height));
		}
	}
}

void GFX::Cleanup()
{
	pSFWindow = nullptr;
}

Vector2 GFX::Project(const Vector2& nPos, bool bPreClamp)
{
	return pSFWindow ? pSFWindow->Project(nPos, bPreClamp) : nPos;
}

Vector2 GFX::GetViewSize()
{
	return pSFWindow ? pSFWindow->GetViewSize() : Vector2::Zero;
}

const Map<u32, SFWindowSize>& GFX::GetValidWindowSizes()
{
	return windowSizes;
}

SFWindowSize* GFX::TryGetWindowSize(u32 height)
{
	auto search = windowSizes.find(height);
	if (search != windowSizes.end())
	{
		return &search->second;
	}
	return nullptr;
}
} // namespace LittleEngine
