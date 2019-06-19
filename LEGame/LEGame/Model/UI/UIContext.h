#pragma once
#include "Core/Asserts.h"
#include "Core/CoreTypes.h"
#include "Core/Delegate.h"
#include "SFMLAPI/Rendering/SFLayerID.h"
#include "LittleEngine/Input/LEInput.h"
#include "LEGame/Model/UI/UIObject.h"
#include "LEGame/Model/UI/UIGameStyle.h"
#include "LEGame/Model/GameManager.h"

namespace LittleEngine
{
// \brief Controller for a number of UIWidgets: allows player to cycle through and interact with all of them
class UIContext : public UIObject
{
private:
	struct MBState
	{
		bool bEnterPressed = false;
		bool bEnterReleased = false;
	};

public:
	using OnCancelled = Core::Delegate<>;
private:
	using UUIWidget = UPtr<class UIWidget>;
	using UUIElement = UPtr<class UIElement>;

public:
	bool m_bAutoDestroyOnCancel = false;
protected:
	UIElement* m_pRootElement = nullptr;
private:
	UPtr<class UIWidgetMatrix> m_uUIWidgets;
	Vec<UUIElement> m_uiElements;
	Vec<LEInput::Token> m_inputTokens;
	OnCancelled m_onCancelledDelegate;
	MBState m_mbState;
	UIWidget* m_pPointerOver = nullptr;
	bool m_bInteracting = false;

public:
	UIContext();
	~UIContext() override;

	template <typename T>
	T* AddWidget(String name, struct UIWidgetStyle* pStyleToCopy = nullptr, bool bNewColumn = false);
	template <typename T>
	T* AddElement(String name, struct UITransform* pParent = nullptr, s32 layerDelta = 0);

	void SetActive(bool bActive, bool bResetSelection = true);
	void ResetSelection();
	UIWidget* GetSelected();
	UIElement* GetRoot() const;
	OnCancelled::Token SetOnCancelled(OnCancelled::Callback callback, bool bAutoDestroy);
	void Destruct();

	void Tick(Time dt = Time::Zero) override;

protected:
	virtual void OnCreated();
	virtual void OnDestroying();

	bool OnInput(const LEInput::Frame& frame);

	void OnUp();
	void OnDown();
	void OnLeft();
	void OnRight();
	void OnEnterPressed();
	void OnEnterReleased(bool bInteract);
	void OnBackReleased();

private:
	void OnCreate(String name, LayerID rootLayer);
	LayerID GetMaxLayer() const;

	friend class UIManager;
	friend class UIWidget;
};

template <typename T>
T* UIContext::AddWidget(String name, UIWidgetStyle* pStyleToCopy, bool bNewColumn)
{
	Assert(g_pGameManager, "GameManager is null!");
	static_assert(std::is_base_of<UIWidget, T>::value, "T must derive from UIWidget.");
	UPtr<T> uT = MakeUnique<T>();
	T* pT = uT.get();
	UIWidgetStyle defaultStyle = UIGameStyle::GetStyle("");
	if (!pStyleToCopy)
	{
		pStyleToCopy = &defaultStyle;
	}
	pStyleToCopy->baseLayer = static_cast<LayerID>(m_pRootElement->m_layer + 1);
	uT->OnCreate(std::move(name), *this, pStyleToCopy);
	m_uUIWidgets->EmplaceWidget(std::move(uT), bNewColumn);
	LOG_D("%s constructed", pT->LogNameStr());
	return pT;
}

template <typename T>
T* UIContext::AddElement(String name, UITransform* pParent, s32 layerDelta)
{
	Assert(g_pGameManager, "GameManager is null!");
	LayerID layer = LAYER_UI;
	static_assert(std::is_base_of<UIElement, T>::value, "T must derive from UIWidget!");
 	if (m_pRootElement)
	{
		layer = static_cast<LayerID>(m_pRootElement->m_layer + 1);
	}
	layer = static_cast<LayerID>(layer + layerDelta);
	UPtr<T> uT = MakeUnique<T>(layer, false);
	if (!pParent && m_pRootElement)
	{
		pParent = &m_pRootElement->m_transform;
	}
	uT->OnCreate(*g_pGameManager->Context(), std::move(name), pParent);
	
	T* pT = uT.get();
	LOG_D("%s constructed", pT->LogNameStr());
	m_uiElements.push_back(std::move(uT));
	return pT;
}
} // namespace LittleEngine
