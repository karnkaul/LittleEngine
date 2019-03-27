#pragma once
#include "CoreTypes.h"
#include "LittleEngine/UI/UIObject.h"
#include "LittleEngine/UI/UIStyle.h"

namespace LittleEngine
{
enum class UIWidgetState
{
	Uninteractable,
	NotSelected,
	Selected,
	Interacting
};

// \brief Base UI interactable class: must be used in conjunction with an owning UIContext
class UIWidget : public UIObject
{
protected:
	UIWidgetStyle m_style;
	Vec<UPtr<class UIElement>> m_uiElements;
	class UIContext* m_pOwner = nullptr;
	UIWidgetState m_state = UIWidgetState::NotSelected;
	UIWidgetState m_prevState;

public:
	UIWidget();
	UIWidget(const String& name);
	virtual ~UIWidget();

	template <typename T>
	UIElement* AddElement(const String& name = "", struct UITransform* pParent = nullptr);

	UIWidgetStyle& GetStyle();

public:
	virtual void SetStyle(const UIWidgetStyle& style);
	virtual void SetInteractable(bool bInteractable) = 0;
	bool IsInteractable() const;

protected:
	void SetState(UIWidgetState state);

protected:
	virtual void OnSelected() = 0;
	virtual void OnDeselected() = 0;
	virtual void OnInteractStart() = 0;
	virtual void OnInteractEnd(bool bInteract) = 0;
	virtual void Tick(Time dt) override;

private:
	void InitWidget(UIContext& owner, UIWidgetStyle* pStyleToCopy);
	void InitElement(UIElement* pNewElement, UITransform* pParent);
	LayerID GetMaxLayer() const;

	virtual void OnInitWidget();

	friend class UIContext;
};

template <typename T>
UIElement* UIWidget::AddElement(const String& name, UITransform* pParent)
{
	static_assert(std::is_base_of<UIElement, T>::value,
				  "T must derive from UIElement. Check Output Window for erroneous call");
	UPtr<T> uT = MakeUnique<T>(name);
	T* pT = uT.get();
	InitElement(pT, pParent);
	m_uiElements.push_back(std::move(uT));
	return pT;
}
} // namespace LittleEngine
