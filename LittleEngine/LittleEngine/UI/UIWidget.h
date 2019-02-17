#pragma once
#include "CoreTypes.h"
#include "LittleEngine/UI/UIElement.h"

namespace LittleEngine
{
// \brief Base UI interactable class: must be used in conjunction with an owning UIContext
class UIWidget : public UIObject
{
private:
	Vector<UPtr<UIElement>> m_uiElements;
	class UIContext* m_pOwner = nullptr;
	LayerID m_rootLayer;

public:
	UIWidget();
	UIWidget(const String& name);
	virtual ~UIWidget();

	template <typename T>
	UIElement* AddElement(const String& name = "")
	{
		static_assert(std::is_base_of<UIElement, T>::value,
					  "T must derive from UIElement. Check Output Window for erroneous call");
		UPtr<T> uT = MakeUnique<T>(name);
		T* pT = uT.get();
		InitElement(pT);
		m_uiElements.push_back(std::move(uT));
		return pT;
	}

	virtual void OnSelected() = 0;
	virtual void OnDeselected() = 0;
	virtual void OnInteractStart() = 0;
	virtual void OnInteractEnd() = 0;

	virtual void Tick(Time dt) override;

private:
	void InitWidget(UIContext& owner, LayerID rootLayer);
	void InitElement(UIElement* pNewElement);

	friend class UIContext;
};
} // namespace LittleEngine