#include <array>
#include "Core/LECoreGame/LECoreUtils/Utils.h"
#include "Tweakable.h"
#if ENABLED(TWEAKABLES)
namespace LittleEngine::Debug
{
Tweakable::Tweakable(std::string id, TweakType type, std::string value /* = "" */, void* pTarget /* = nullptr */)
	: m_pTarget(pTarget), m_value(std::move(value)), m_type(type)
{
	static std::array<std::string_view, 4> suffix = {"", "_f32", "_s32", "_b"};
	m_id = std::move(id);
	m_id += suffix[ToIdx(m_type)];
	TweakManager::Instance()->m_tweakables.emplace(m_id, this);
}

Tweakable::~Tweakable()
{
	TweakManager::Instance()->m_tweakables.erase(m_id);
}

void Tweakable::Set(std::string stringValue)
{
	switch (m_type)
	{
	default:
	case TweakType::String:
		SyncString(std::move(stringValue));
		break;

	case TweakType::F32:
		SyncF32(std::move(stringValue));
		break;

	case TweakType::S32:
		SyncS32(std::move(stringValue));
		break;

	case TweakType::Bool:
		SyncBool(std::move(stringValue));
		break;
	}
	if (m_callback)
	{
		m_callback(m_value);
	}
}

void Tweakable::Bind(void* pVar)
{
	m_pTarget = pVar;
}

void Tweakable::BindCallback(std::function<void(std::string_view)> callback)
{
	m_callback = std::move(callback);
}

void Tweakable::SyncString(std::string rawValue)
{
	m_value = std::move(rawValue);
	if (m_pTarget)
	{
		auto pString = reinterpret_cast<std::string*>(m_pTarget);
		*pString = m_value;
	}
}

void Tweakable::SyncF32(std::string rawValue)
{
	f32 value = Strings::ToF32(rawValue);
	m_value = Strings::ToString(value);
	if (m_pTarget)
	{
		auto pF32 = reinterpret_cast<f32*>(m_pTarget);
		*pF32 = value;
	}
}

void Tweakable::SyncS32(std::string rawValue)
{
	s32 value = Strings::ToS32(rawValue);
	m_value = Strings::ToString(value);
	if (m_pTarget)
	{
		auto pS32 = reinterpret_cast<s32*>(m_pTarget);
		*pS32 = value;
	}
}

void Tweakable::SyncBool(std::string rawValue)
{
	bool bValue = Strings::ToBool(rawValue);
	m_value = Strings::ToString(bValue);
	if (m_pTarget)
	{
		auto pBool = reinterpret_cast<bool*>(m_pTarget);
		*pBool = bValue;
	}
}

TweakManager* TweakManager::Instance()
{
	static TweakManager manager;
	return &manager;
}

Tweakable* TweakManager::Find(std::string_view id) const
{
	auto iter = m_tweakables.find(id);
	if (iter != m_tweakables.end())
	{
		return iter->second;
	}
	return nullptr;
}
} // namespace LittleEngine::Debug
#endif
