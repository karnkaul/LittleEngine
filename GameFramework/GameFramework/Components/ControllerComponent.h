#pragma once
#include "LittleEngine/Game/Component.h"

namespace LittleEngine
{
class ControllerComponent : public Component
{
public:
	Fixed m_angularSpeed = Fixed::OneThird;
	Fixed m_linearSpeed = Fixed::One;

protected:
	Vector2 m_displacement;
	Fixed m_rotation;
	class RenderComponent* m_pRenderComponent = nullptr;

public:
	void Reset();

	TimingType GetComponentTiming() const override;
	void OnCreated() override;
	void Tick(Time dt) override;
	void SetEnabled(bool bEnabled) override;

private:
	virtual bool OnInput(const EngineInput::Frame& frame);
};
} // namespace LittleEngine
