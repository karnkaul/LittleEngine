#pragma once
#include "UIObject.h"
#include "UIContext.h"

namespace LittleEngine
{
// TODO: for handling multiple UIContexts
class UIManager final : public UIObject
{
public:
private:
	std::unique_ptr<UIContext> m_uActiveContext;

public:
	UIManager(World& owner);
	~UIManager();

	template <typename T>
	T* SpawnContext()
	{
		static_assert(std::is_base_of<UIContext, T>::value, "T must derive from UIContext!");
		m_uActiveContext = MakeUnique<T>();
		m_uActiveContext->InitContext();
		m_uActiveContext->SetActive(false);
		return dynamic_cast<T*>(m_uActiveContext.get());
	}

	UIContext* GetActiveContext() const;

	virtual void Tick(Time dt) override;
};
} // namespace LittleEngine
