#pragma once
#include "Model/World/Component.h"

namespace LittleEngine
{
class ControllerComponent : public AComponent
{
#if defined(DEBUGGING)
public:
	static bool s_bShowJoystickOrientation;
	static Colour s_orientationColour;
	static Vector2 s_orientationWidthHeight;

private:
	class SFRect* m_pRect = nullptr;
#endif
public:
	static Fixed s_orientationEpsilon;

public:
	Fixed m_angularSpeed = Fixed::OneThird;
	Fixed m_linearSpeed = Fixed::One;

protected:
	Vector2 m_displacement;
	Fixed m_rotation;
	class RenderComponent* m_pRenderComponent = nullptr;
	bool m_bKeyInput = false;

public:
	ControllerComponent();
	~ControllerComponent() override;

protected:
	void OnCreated() override;

public:
	void Tick(Time dt) override;

public:
	TimingType Timing() const override;
	void SetEnabled(bool bEnabled) override;

public:
	void Reset();

private:
	bool OnInput(const LEInput::Frame& frame);
};
} // namespace LittleEngine
