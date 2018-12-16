#pragma once
#include <vector>
#include <memory>
#include "../UIObject.h"
#include "../UIElement.h"

namespace LittleEngine {
	// \brief Base UI interactable class: must be used in conjunction with an owning UIContext
	class UIWidget : public UIObject {
	private:
		std::vector<std::unique_ptr<UIElement>> m_uiElements;
		class Level* m_pLevel = nullptr;
		class UIContext* m_pOwner = nullptr;

	public:
		UIWidget();
		UIWidget(const std::string& name);
		virtual ~UIWidget();

		template<typename T>
		UIElement* AddUIElement() {
			static_assert(std::is_base_of<UIElement, T>::value, "T must derive from UIElement. Check Output Window for erroneous call");
			std::unique_ptr<T> uT = std::make_unique<T>();
			uT->InitUIElement(*m_pLevel);
			T* pT = uT.get();
			m_uiElements.push_back(std::move(uT));
			return pT;
		}

		void InitWidget(Level& level, UIContext& owner);

		virtual void OnSelected() = 0;
		virtual void OnDeselected() = 0;
		virtual void OnInteractStart() = 0;
		virtual void OnInteractEnd() = 0;

		virtual void Tick(const Fixed& deltaMS) override;
		virtual void Render() override;
	};
}
