#pragma once
#include "Core/Asserts.h"
#include "Core/CoreTypes.h"
#include "Core/Delegate.h"
#include "SFMLAPI/Rendering/LayerID.h"
#include "Engine/Input/LEInput.h"
#include "UIWidget.h"
#include "UIGameStyle.h"
#include "UIWidgetMatrix.h"
#include "Model/GameManager.h"

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
	using UUIWidget = UPtr<UIWidget>;
	using UUIElement = UPtr<UIElement>;

private:
	bool m_bPersistent = false;

public:
	bool m_bAutoDestroyOnCancel = false;

protected:
	SPtr<s32> m_ptrToken;
	UIElement* m_pRoot = nullptr;

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
	UIWidget* Selected();
	UIElement* Root() const;
	OnCancelled::Token SetOnCancelled(OnCancelled::Callback callback, bool bAutoDestroy);
	void Destruct();

	void Tick(Time dt = Time::Zero) override;

	bool IsPersistent() const;
	void SetPersistent(bool bPersistent);
	void Regenerate(LayerID newLayer);

protected:
	virtual void OnCreated();
	virtual void OnActivated();
	virtual void OnDestroying();
	virtual void OnEnabling(bool bEnabled);

	bool OnInput(const LEInput::Frame& frame);

	void OnUp();
	void OnDown();
	void OnLeft();
	void OnRight();
	void OnEnterPressed();
	void OnEnterReleased(bool bInteract);
	void OnBackReleased();

private:
	void OnCreate(String id, LayerID rootLayer);
	void SetEnabled(bool bSetEnabled);
	LayerID MaxLayer() const;

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
	pStyleToCopy->baseLayer = static_cast<LayerID>(ToS32(m_pRoot->m_layer) + 1);
	uT->OnCreate(std::move(name), *this, pStyleToCopy);
	m_uUIWidgets->EmplaceWidget(std::move(uT), bNewColumn);
	LOG_D("%s constructed", pT->LogName().data());
	return pT;
}

template <typename T>
T* UIContext::AddElement(String name, UITransform* pParent, s32 layerDelta)
{
	Assert(g_pGameManager, "GameManager is null!");
	LayerID layer = LayerID::UI;
	static_assert(std::is_base_of<UIElement, T>::value, "T must derive from UIWidget!");
	if (m_pRoot)
	{
		layer = static_cast<LayerID>(ToS32(m_pRoot->m_layer) + 1);
	}
	layer = static_cast<LayerID>(ToS32(layer) + layerDelta);
	UPtr<T> uT = MakeUnique<T>(layer, false);
	if (!pParent && m_pRoot)
	{
		pParent = &m_pRoot->m_transform;
	}
	uT->OnCreate(*g_pGameManager->Context(), std::move(name), pParent);

	T* pT = uT.get();
	LOG_D("%s constructed", pT->LogName().data());
	m_uiElements.push_back(std::move(uT));
	return pT;
}
} // namespace LittleEngine
