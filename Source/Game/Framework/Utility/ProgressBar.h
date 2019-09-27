#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/LayerID.h"

namespace LittleEngine
{
class ProgressBar : private NoCopy
{
public:
	Vector2 m_size;

private:
	class Quad* m_pQuad = nullptr;

public:
	ProgressBar(LayerID layer);
	virtual ~ProgressBar();

public:
	virtual void SetProgress(Fixed progress, bool bImmediate = false);

public:
	ProgressBar* SetEnabled(bool bEnabled);

	Quad* GetQuad() const;
};
}
