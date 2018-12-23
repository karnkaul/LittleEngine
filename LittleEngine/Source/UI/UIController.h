#pragma once
#include "UIObject.h"
#include "Contexts/UIContext.h"

namespace LittleEngine {
	// TODO: for handling multiple UIContexts
	class UIController final : public UIObject {
	public:
	private:
		class Level* m_pLevel;
		std::unique_ptr<UIContext> m_uActiveContext;

	public:
		UIController(Level& level);
		~UIController();

		template<typename T>
		T* SpawnContext() {
			static_assert(std::is_base_of<UIContext, T>::value, "T must derive from UIContext!");
			m_uActiveContext = std::make_unique<T>();
			m_uActiveContext->InitContext(*m_pLevel);
			m_uActiveContext->SetActive(false);
			return dynamic_cast<T*>(m_uActiveContext.get());
		}

		UIContext* GetActiveContext() const;

		virtual void Tick(const Fixed& deltaMS) override;
		virtual void Render() override;
	private:

	};
}
