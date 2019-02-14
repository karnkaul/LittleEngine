#pragma once
#include "LittleEngine/Game/Component.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"

namespace LittleEngine
{
class RenderComponent : public Component
{
public:
	SFPrimitive* m_pSFPrimitive;

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
