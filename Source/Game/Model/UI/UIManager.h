#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/Rendering/LayerID.h"
#include "UIContext.h"

namespace LittleEngine
{
class UIManager final
{
public:
private:
	List<UPtr<class UIContext>> m_contexts;
	UMap<String, UPtr<UIContext>> m_inactive;

public:
	UIManager();
	~UIManager();

	template <typename T>
	T* PushContext(String id);
	UIContext* Active() const;

private:
	template <typename T>
	UPtr<T> Inactive(const String& id);
	template <typename T>
	UPtr<T> CreateContext(String id);

	void Reset();
	void Tick(Time dt);

	void InitContext(UIContext& context, String id, LayerID baseLayer);
	void DisableContext(UIContext& context);

private:
	friend class GameManager;
};

template <typename T>
T* UIManager::PushContext(String id)
{
	static_assert(IsDerived<UIContext, T>(), "T must derive from UIContext!");

	// Check if id matches an existing context
	if (!m_contexts.empty())
	{
		auto head = m_contexts.rbegin();
		auto& uHead = *head;
		if (uHead->m_name == id)
		{
			// If Active context is a match, nothing else to do
			return dynamic_cast<T*>(uHead.get());
		}
		uHead->SetActive(false);
		for (auto iter = head; iter != m_contexts.rend(); ++iter)
		{
			auto& uContext = *iter;
			if (uContext->m_name == id)
			{
				LayerID baseLayer = static_cast<LayerID>(uHead->MaxLayer() + 2);
				std::swap(uHead, uContext);
				uHead->Regenerate(baseLayer);
				uHead->SetActive(true);
				return dynamic_cast<T*>(uHead.get());
			}
		}
	}

	UPtr<T> uT = Inactive<T>(id);
	if (!uT)
	{
		uT = CreateContext<T>(std::move(id));
	}
	uT->SetActive(true);
	T* pT = dynamic_cast<T*>(uT.get());
	m_contexts.emplace_back(std::move(uT));
	LOG_D("%s pushed on to UIStack", pT->LogNameStr());
	return pT;
}

template <typename T>
UPtr<T> UIManager::Inactive(const String& id)
{
	auto search = m_inactive.find(id);
	if (search != m_inactive.end())
	{
		auto uContext = std::move(search->second);
		Assert(dynamic_cast<T*>(uContext.get()), "Type-ID mismatch!");
		m_inactive.erase(search);
		LayerID baseLayer = m_contexts.empty() ? LAYER_UI : static_cast<LayerID>(m_contexts.back()->MaxLayer() + 2);
		uContext->Regenerate(baseLayer);
		return UPtr<T>(dynamic_cast<T*>(uContext.release()));
	}
	return {};
}

template <typename T>
UPtr<T> UIManager::CreateContext(String id)
{
	UPtr<T> uT = MakeUnique<T>();
	LayerID baseLayer = m_contexts.empty() ? LAYER_UI : static_cast<LayerID>(m_contexts.back()->MaxLayer() + 2);
	InitContext(*uT, std::move(id), baseLayer);
	return (uT);
}

} // namespace LittleEngine
