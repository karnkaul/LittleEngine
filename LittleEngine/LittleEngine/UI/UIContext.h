#pragma once
#include "CoreTypes.h"
#include "Delegate.hpp"
#include "LittleEngine/UI/UIWidget.h"
#include "LittleEngine/UI/UIWidgetMatrix.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
// \brief Controller for a number of UIWidgets: allows player to cycle through and interact with all of them
class UIContext : public UIObject
{
private:
	using UUIWidget = UPtr<UIWidget>;
	using UUIElement = UPtr<class UIElement>;

public:
	bool m_bAutoDestroyOnCancel = false;

protected:
	UIElement* m_pRootElement = nullptr;

private:
	UIWidgetMatrix m_uiWidgets;
	Vector<UUIElement> m_uiElements;
	Vector<EngineInput::Token> m_inputTokens;
	Core::Delegate<> m_onCancelledDelegate;

public:
	UIContext();
	UIContext(const String& name);
	virtual ~UIContext();

	template <typename T>
	T* AddWidget(const String& name, bool bNewColumn = false)
	{
		static_assert(std::is_base_of<UIWidget, T>::value, "T must derive from UIWidget.");
		UPtr<T> uT = MakeUnique<T>(name);
		T* pT = uT.get();
		uT->InitWidget(*this);
		m_uiWidgets.EmplaceWidget(std::move(uT), bNewColumn);
		return pT;
	}

	template <typename T>
	T* AddElement(const String& name, UITransform* pParent = nullptr)
	{
		static_assert(std::is_base_of<UIElement, T>::value, "T must derive from UIWidget!");
		UPtr<T> uT = MakeUnique<T>(name);
		if (!pParent && m_pRootElement)
			pParent = &m_pRootElement->m_transform;
		uT->InitElement(pParent);
		if (m_pRootElement)
			uT->m_layer = static_cast<LayerID>(m_pRootElement->m_layer + 1);
		T* pT = uT.get();
		m_uiElements.push_back(std::move(uT));
		return pT;
	}

	void InitContext();
	void SetActive(bool bActive);
	void ResetSelection();
	UIWidget* GetSelected();
	UIElement* GetRootElement() const;
	Core::Delegate<>::Token SetOnCancelled(Core::Delegate<>::Callback Callback, bool bAutoDestroy);
	void Destruct();

	virtual void Tick(Time dt) override;

protected:
	virtual void OnDestroying();

	bool OnInput(const EngineInput::Frame& frame);

	void OnUp();
	void OnDown();
	void OnLeft();
	void OnRight();
	void OnEnterPressed();
	void OnEnterReleased();
	void OnBackReleased();
	void Discard();

private:
	friend class UIManager;
};
} // namespace LittleEngine
