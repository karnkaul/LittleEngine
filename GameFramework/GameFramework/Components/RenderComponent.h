#pragma once
#include "LittleEngine/Game/Component.h"

namespace LittleEngine
{
class RenderComponent : public AComponent
{
public:
	class SFPrimitive* m_pSFPrimitive = nullptr;

public:
	~RenderComponent() override;

	TimingType GetComponentTiming() const override;
	void OnCreated() override;
	void SetEnabled(bool bEnabled) override;
	void Tick(Time dt) override;

private:
	void UpdatePrimitive();
};
} // namespace LittleEngine
