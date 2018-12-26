#pragma once
#include <memory>
#include "Delegate.hpp"
#include "../UIObject.h"
#include "../Widgets/UIWidget.h"
#include "../UIWidgetMatrix.h"
#include "Engine/Input/InputHandler.h"

namespace LittleEngine {
	// \brief Controller for a number of UIWidgets: allows player to cycle through and interact with all of them
	class UIContext : public UIObject {
	private:
		using UUIWidget = std::unique_ptr<UIWidget>;
		using UUIElement = std::unique_ptr<class UIElement>;
	public:
		bool m_bAutoDestroyOnCancel = false;
	protected:
		UIElement* m_pRootElement = nullptr;
	private:
		UIWidgetMatrix m_uiWidgets;
		std::vector<UUIElement> m_uiElements;
		std::vector<OnInput::Token> m_inputTokens;
		OnInput m_onCancelledDelegate;
		class Level* m_pLevel = nullptr;
		
	public:
		UIContext();
		UIContext(const std::string& name);
		virtual ~UIContext();

		template <typename T>
		T* AddWidget(const std::string& name, bool bNewColumn = false) {
			static_assert(std::is_base_of<UIWidget, T>::value, "T must derive from UIWidget.");
			std::unique_ptr<T> uT = std::make_unique<T>(name);
			T* pT = uT.get();
			uT->InitWidget(*m_pLevel, *this);
			m_uiWidgets.EmplaceWidget(std::move(uT), bNewColumn);
			return pT;
		}

		template<typename T>
		T* AddElement(const std::string& name, UITransform* pParent = nullptr) {
			static_assert(std::is_base_of<UIElement, T>::value, "T must derive from UIWidget!");
			std::unique_ptr<T> uT = std::make_unique<T>(name);
			if (!pParent && m_pRootElement) pParent = &m_pRootElement->m_transform;
			uT->InitElement(*m_pLevel, pParent);
			T* pT = uT.get();
			m_uiElements.push_back(std::move(uT));
			return pT;
		}

		void InitContext(Level& level);
		void SetActive(bool bActive);
		void ResetSelection();
		UIWidget* GetSelected();
		UIElement* GetRootElement() const;
		OnInput::Token SetOnCancelled(OnInput::Callback Callback, bool bAutoDestroy);
		void Destruct();

		virtual void Tick(const Fixed& deltaMS) override;
		virtual void Render() override;

	protected:
		virtual void OnDestroying();
		
		void OnUp();
		void OnDown();
		void OnLeft();
		void OnRight();
		void OnEnterPressed();
		void OnEnterReleased();
		void OnReturnReleased();
		void Discard();

	private:
		friend class UIController;
	};
}
