#include "Model/UI/UIText.h"
#include "Engine/Repository/LERepository.h"

namespace LittleEngine
{
class ILoadingHUD
{
public:
	virtual ~ILoadingHUD() = default;

public:
	virtual SFText& Title() const = 0;
	virtual SFText* Subtitle() const = 0;
	virtual class Quad* Spinner() const = 0;

	virtual void Tick(Time dt, Fixed progress) = 0;
	virtual void Reset() = 0;
	virtual void SetEnabled(bool bEnabled) = 0;

	// Call in constructor to preload required Assets
	template <typename T>
	T* PreloadAsset(const String& id) const;
};

template <typename T>
T* ILoadingHUD::PreloadAsset(const String& id) const
{
	return g_pRepository->template Preload<T>(id);
}
} // namespace LittleEngine
