#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/LayerID.h"

namespace LittleEngine
{
class UIManager final
{
public:
private:
	Vec<UPtr<class UIContext>> m_uContexts;

public:
	UIManager();
	~UIManager();

	template <typename T>
	T* PushContext(String name = "Untitled");
	UIContext* GetActiveContext() const;

	void Tick(Time dt);

private:
	void InitContext(UIContext* pContext, String name, LayerID baseLayer);
};

template <typename T>
T* UIManager::PushContext(String name)
{
	static_assert(IsDerived<UIContext, T>(), "T must derive from UIContext!");
	if (!m_uContexts.empty())
	{
		auto& uHead = m_uContexts.back();
		uHead->SetActive(false);
	}
	UPtr<T> uT = MakeUnique<T>();
	LayerID baseLayer = m_uContexts.empty() ? LAYER_UI : static_cast<LayerID>(m_uContexts.back()->GetMaxLayer() + 2);
	InitContext(uT.get(), std::move(name), baseLayer);
	uT->SetActive(true);
	T* pT = dynamic_cast<T*>(uT.get());
	m_uContexts.emplace_back(std::move(uT));
	LOG_D("%s pushed on to UIStack", pT->LogNameStr());
	return pT;
}

} // namespace LittleEngine
