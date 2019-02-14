#include "stdafx.h"
#include "GFX.h"
#include "SFMLAPI/Windowing/SFWindow.h"

namespace LittleEngine
{
namespace
{
const SFWindow* pSFWindow = nullptr;
}

void GFX::Init(const SFWindow& sfWindow)
{
	pSFWindow = &sfWindow;
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
} // namespace LittleEngine
