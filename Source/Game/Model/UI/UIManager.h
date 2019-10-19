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
	std::list<std::unique_ptr<class UIContext>> m_contexts;
	std::unordered_map<std::string, std::unique_ptr<UIContext>> m_inactive;

public:
	UIManager();
	~UIManager();

	template <typename T>
	T* PushContext(std::string id);
	UIContext* Active() const;

private:
	template <typename T>
	std::unique_ptr<T> Inactive(const std::string& id);
	template <typename T>
	std::unique_ptr<T> CreateContext(std::string id);

	void Clear();
	void Tick(Time dt);

	void InitContext(UIContext& context, std::string id, LayerID baseLayer);
	void DisableContext(UIContext& context);

private:
	friend class GameManager;
};

template <typename T>
T* UIManager::PushContext(std::string id)
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
				LayerID baseLayer = static_cast<LayerID>(ToS32(uHead->MaxLayer()) + 2);
				std::swap(uHead, uContext);
				uHead->Regenerate(baseLayer);
				uHead->SetActive(true);
				return dynamic_cast<T*>(uHead.get());
			}
		}
	}

	std::unique_ptr<T> uT = Inactive<T>(id);
	if (!uT)
	{
		uT = CreateContext<T>(std::move(id));
	}
	uT->SetActive(true);
	T* pT = dynamic_cast<T*>(uT.get());
	m_contexts.emplace_back(std::move(uT));
	LOG_D("%s pushed on to UIStack", pT->LogName().data());
	return pT;
}

template <typename T>
std::unique_ptr<T> UIManager::Inactive(const std::string& id)
{
	auto search = m_inactive.find(id);
	if (search != m_inactive.end())
	{
		auto uContext = std::move(search->second);
		Assert(dynamic_cast<T*>(uContext.get()), "Type-ID mismatch!");
		m_inactive.erase(search);
		LayerID baseLayer = m_contexts.empty() ? LayerID::UI : static_cast<LayerID>(ToS32(m_contexts.back()->MaxLayer()) + 2);
		uContext->Regenerate(baseLayer);
		return std::unique_ptr<T>(dynamic_cast<T*>(uContext.release()));
	}
	return {};
}

template <typename T>
std::unique_ptr<T> UIManager::CreateContext(std::string id)
{
	std::unique_ptr<T> uT = std::make_unique<T>();
	LayerID baseLayer = m_contexts.empty() ? LayerID::UI : static_cast<LayerID>(ToS32(m_contexts.back()->MaxLayer()) + 2);
	InitContext(*uT, std::move(id), baseLayer);
	return (uT);
}

} // namespace LittleEngine
