#pragma once
#include "LittleEngine/Game/Component.h"

namespace LittleEngine
{
class RenderComponent : public Component
{
public:
	class SFPrimitive* m_pSFPrimitive;

public:
	virtual ~RenderComponent();

	virtual TimingType GetComponentTiming() const override;
	virtual void OnCreated() override;
	virtual void SetEnabled(bool bEnabled) override;
	virtual void Tick(Time dt) override;

private:
	void UpdatePrimitive();
};
} // namespace LittleEngine
