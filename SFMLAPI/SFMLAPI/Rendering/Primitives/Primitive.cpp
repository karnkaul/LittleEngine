#include "stdafx.h"
#include "Primitive.h"
#include "SFMLAPI/System/SFTypes.h"
#include "SFMLAPI/Rendering/SFShader.h"

namespace LittleEngine
{
void APrimitive::State::Reconcile()
{
	sfPosition.min = sfPosition.max;
	sfOrientation.min = sfOrientation.max;
	sfScale.min = sfScale.max;
	sfPrimaryColour.min = sfPrimaryColour.max;
	sfSecondaryColour.min = sfSecondaryColour.max;
}

APrimitive::APrimitive(LayerID layer) : m_layer(layer)
{
}

APrimitive::~APrimitive() = default;

void APrimitive::ReconcileGameState()
{
	m_gameState.Reconcile();
}

void APrimitive::SwapState()
{
	m_renderState = m_gameState;
	OnSwapState();
}

void APrimitive::UpdateRenderState(Fixed alpha)
{
	if (m_bStatic || !m_renderState.bEnabled)
	{
		return;
	}

	OnUpdateRenderState(alpha);

	if (m_bMakeStatic)
	{
		m_bStatic = true;
		m_bMakeStatic = false;
	}
}

void APrimitive::Destroy()
{
	m_bDestroyed = true;
}

void APrimitive::Draw(SFViewport& viewport, Fixed alpha)
{
	UpdateRenderState(alpha);
	sf::RenderStates states;
	if (m_pShader)
	{
		states.shader = &m_pShader->m_sfShader;
		m_pShader->PreDraw(this);
	}
	OnDraw(viewport, states);
	m_bRendered = true;
}

APrimitive* APrimitive::SetPosition(Vector2 worldPosition, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfPosition.Reset(WorldToScreen(worldPosition));
	}
	else
	{
		m_gameState.sfPosition.Update(WorldToScreen(worldPosition));
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

APrimitive* APrimitive::SetOrientation(Fixed worldOrientation, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfOrientation.Reset(WorldToScreen(worldOrientation));
	}
	else
	{
		m_gameState.sfOrientation.Update(WorldToScreen(worldOrientation));
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

APrimitive* APrimitive::SetScale(Vector2 worldScale, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfScale.Reset(worldScale);
	}
	else
	{
		m_gameState.sfScale.Update(worldScale);
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

APrimitive* APrimitive::SetPrimaryColour(Colour sfColour, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfPrimaryColour.Reset(sfColour);
	}
	else
	{
		m_gameState.sfPrimaryColour.Update(sfColour);
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

APrimitive* APrimitive::SetSecondaryColour(Colour sfColour, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.sfSecondaryColour.Reset(sfColour);
	}
	else
	{
		m_gameState.sfSecondaryColour.Update(sfColour);
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

APrimitive* APrimitive::SetEnabled(bool bEnabled)
{
	m_gameState.bEnabled = bEnabled;
	return this;
}

APrimitive* APrimitive::SetPivot(Vector2 pivot)
{
	m_gameState.pivot = pivot;
	return this;
}

APrimitive* APrimitive::SetShader(SFShader* pShader)
{
	m_pShader = pShader;
	return this;
}

APrimitive* APrimitive::SetStatic(bool bStatic)
{
	if (bStatic)
	{
		if (!m_bStatic)
		{
			m_bMakeStatic = true;
		}
	}
	else
	{
		m_bStatic = m_bMakeStatic = false;
	}
	return this;
}

bool APrimitive::IsDestroyed() const
{
	return m_bDestroyed;
}

bool APrimitive::IsEnabled() const
{
	return m_gameState.bEnabled;
}

bool APrimitive::IsStatic() const
{
	return m_bStatic;
}

Vector2 APrimitive::GetPosition() const
{
	return ScreenToWorld(m_gameState.sfPosition.max);
}

Fixed APrimitive::GetOrientation() const
{
	return ScreenToWorld(m_gameState.sfOrientation.max);
}

Vector2 APrimitive::GetScale() const
{
	return m_gameState.sfScale.max;
}

Colour APrimitive::GetPrimaryColour() const
{
	return m_gameState.sfPrimaryColour.max;
}

Colour APrimitive::GetSecondaryColour() const
{
	return m_gameState.sfSecondaryColour.max;
}
} // namespace LittleEngine