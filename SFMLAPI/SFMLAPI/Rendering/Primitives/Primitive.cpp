#include "stdafx.h"
#include "Primitive.h"
#include "SFMLAPI/System/SFTypes.h"
#include "SFMLAPI/Rendering/SFShader.h"

namespace LittleEngine
{
void APrimitive::TState::Reconcile()
{
	tPosition.min = tPosition.max;
	tOrientation.min = tOrientation.max;
	tScale.min = tScale.max;
	tColour.min = tColour.max;
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

APrimitive* APrimitive::SetPosition(Vector2 position, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.tPosition.Reset(WorldToScreen(position));
	}
	else
	{
		m_gameState.tPosition.Update(WorldToScreen(position));
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

APrimitive* APrimitive::SetOrientation(Vector2 orientation, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.tOrientation.Reset(WorldToScreen(orientation));
	}
	else
	{
		m_gameState.tOrientation.Update(WorldToScreen(orientation));
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

APrimitive* APrimitive::SetScale(Vector2 scale, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.tScale.Reset(scale);
	}
	else
	{
		m_gameState.tScale.Update(scale);
	}
	if (m_bStatic || m_bMakeStatic)
	{
		ReconcileGameState();
		m_bStatic = false;
		m_bMakeStatic = true;
	}
	return this;
}

APrimitive* APrimitive::SetPrimaryColour(Colour colour, bool bImmediate)
{
	if (bImmediate)
	{
		m_gameState.tColour.Reset(colour);
	}
	else
	{
		m_gameState.tColour.Update(colour);
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
	return ScreenToWorld(m_gameState.tPosition.max);
}

Vector2 APrimitive::GetOrientation() const
{
	return ScreenToWorld(m_gameState.tOrientation.max);
}

Vector2 APrimitive::GetScale() const
{
	return m_gameState.tScale.max;
}

Colour APrimitive::GetPrimaryColour() const
{
	return m_gameState.tColour.max;
}
} // namespace LittleEngine