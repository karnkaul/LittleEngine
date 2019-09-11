#pragma once
#include "Core/CoreTypes.h"
#include "Core/SimpleTime.h"
#include "Model/UI/UIText.h"
#include "ILoadingHUD.h"

namespace LittleEngine
{
class LoadingHUD final : public ILoadingHUD
{
private:
	class Quad* m_pBG = nullptr;
	Quad* m_pRotator = nullptr;
	class SFRect* m_pProgressBar = nullptr;
	class SFText* m_pTitle = nullptr;
	SFText* m_pSubtitle = nullptr;
	Time m_elapsed;
	Fixed m_angle;
	Fixed m_alpha;
	Fixed m_alphaTarget;

public:
	LoadingHUD();
	~LoadingHUD() override;

	SFText& Title() const override;
	SFText* Subtitle() const override;
	Quad* Spinner() const override;

	void Tick(Time dt, Fixed progress) override;
	void Reset() override;
	void SetEnabled(bool bEnabled) override;

private:
	void Update(bool bImmediate);
};
} // namespace LittleEngine
