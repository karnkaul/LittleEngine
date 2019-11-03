#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "Engine/Rendering/LERenderer.h"
#include "ProgressBar.h"
#include "Game/Model/GameManager.h"

namespace LittleEngine
{
ProgressBar::ProgressBar(LayerID layer)
{
	Assert(g_pGameManager, "GameManager is null");
	m_pQuad = g_pGameManager->Renderer()->New<Quad>(layer);
	m_pQuad->SetColour(Colour::White)->SetEnabled(true);
}

ProgressBar::~ProgressBar()
{
	if (m_pQuad)
	{
		m_pQuad->Destroy();
	}
}

void ProgressBar::SetProgress(Fixed progress, bool bImmediate)
{
	Fixed halfHeight = Fixed::OneHalf * m_size.y;
	Vector2 tl(0, halfHeight);
	Vector2 br(progress * m_size.x, -halfHeight);
	m_pQuad->SetModel(Rect2{tl, br}, bImmediate);
}

ProgressBar* ProgressBar::SetEnabled(bool bEnabled)
{
	if (m_pQuad)
	{
		m_pQuad->SetEnabled(bEnabled);
	}
	return this;
}

Quad* ProgressBar::GetQuad() const
{
	return m_pQuad;
}
} // namespace LittleEngine
