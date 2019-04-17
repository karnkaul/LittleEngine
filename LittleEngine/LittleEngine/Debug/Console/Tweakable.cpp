#include "stdafx.h"
#include "Utils.h"
#include "Tweakable.h"
#if ENABLED(TWEAKABLES)
namespace LittleEngine
{
namespace Debug
{
Tweakable::Tweakable(String id, TweakType type, String value /* = "" */, void* pTarget /* = nullptr */)
	: m_value(std::move(value)), m_type(type), m_pTarget(pTarget)
{
	static String suffix[4] = {"", "_f32", "_s32", "_b"};
	m_id = std::move(id) + suffix[m_type];
	TweakManager::Instance()->m_tweakables.emplace(m_id, this);
}

Tweakable::~Tweakable()
{
	TweakManager::Instance()->m_tweakables.erase(m_id);
}

void Tweakable::Set(String stringValue)
{
	switch (m_type)
	{
	default:
	case TweakType::STRING:
		SyncString(std::move(stringValue));
		break;

	case TweakType::F32:
		SyncF32(std::move(stringValue));
		break;

	case TweakType::S32:
		SyncS32(std::move(stringValue));
		break;

	case TweakType::BOOL:
		SyncBool(std::move(stringValue));
		break;
	}
}

void Tweakable::Bind(void* pVar)
{
	m_pTarget = pVar;
}

void Tweakable::SyncString(String rawValue)
{
	m_value = std::move(rawValue);
	if (m_pTarget)
	{
		String* pString = reinterpret_cast<String*>(m_pTarget);
		*pString = m_value;
	}
}

void Tweakable::SyncF32(String rawValue)
{
	f32 value = Strings::ToF32(rawValue);
	m_value = Strings::ToString(value);
	if (m_pTarget)
	{
		f32* pF32 = reinterpret_cast<f32*>(m_pTarget);
		*pF32 = value;
	}
}

void Tweakable::SyncS32(String rawValue)
{
	s32 value = Strings::ToS32(rawValue);
	m_value = Strings::ToString(value);
	if (m_pTarget)
	{
		f32* pF32 = reinterpret_cast<f32*>(m_pTarget);
		*pF32 = value;
	}
}

void Tweakable::SyncBool(String rawValue)
{
	bool bValue = Strings::ToBool(rawValue);
	m_value = Strings::ToString(bValue);
	if (m_pTarget)
	{
		bool* pBool = reinterpret_cast<bool*>(m_pTarget);
		*pBool = bValue;
	}
}

TweakManager* TweakManager::Instance()
{
	static TweakManager manager;
	return &manager;
}

Tweakable* TweakManager::Find(const String& id) const
{
	auto iter = m_tweakables.find(id);
	if (iter != m_tweakables.end())
	{
		return iter->second;
	}
	return nullptr;
}
} // namespace Debug
} // namespace LittleEngine
#endif