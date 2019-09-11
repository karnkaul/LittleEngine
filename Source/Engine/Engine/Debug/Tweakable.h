#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(TWEAKABLES)
#define TweakBool(id, address) LittleEngine::Debug::Tweakable id(#id, LittleEngine::Debug::TweakType::BOOL, "false", address)
#define TweakS32(id, address) LittleEngine::Debug::Tweakable id(#id, LittleEngine::Debug::TweakType::S32, "-1", address)
#define TweakF32(id, address) LittleEngine::Debug::Tweakable id(#id, LittleEngine::Debug::TweakType::F32, "-1.0", address)
#define TweakString(id, address) LittleEngine::Debug::Tweakable id(#id, LittleEngine::Debug::TweakType::STRING, "", address)
#else
#define TweakBool(_disabled0, _disabled1)
#define TweakS32(_disabled0, _disabled1)
#define TweakF32(_disabled0, _disabled1)
#define TweakString(_disabled0, _disabled1)
#endif

#if ENABLED(TWEAKABLES)
namespace LittleEngine
{
namespace Debug
{
enum TweakType
{
	STRING = 0,
	F32 = 1,
	S32 = 2,
	BOOL = 3,
};

class Tweakable final
{
private:
	void* m_pTarget = nullptr;
	std::function<void(const String&)> m_callback;

public:
	String m_id;
	String m_value;
	TweakType m_type = TweakType::STRING;

public:
	Tweakable(String id, TweakType type, String value = "", void* pTarget = nullptr);
	~Tweakable();
	Tweakable(const Tweakable&) = delete;
	Tweakable& operator=(const Tweakable&) = delete;

	void Set(String stringValue);
	void Bind(void* pVar);
	void BindCallback(std::function<void(const String&)> callback);

private:
	void SyncString(String rawValue);
	void SyncF32(String rawValue);
	void SyncS32(String rawValue);
	void SyncBool(String rawValue);
};

class TweakManager
{
public:
	Map<String, Tweakable*> m_tweakables;

	static TweakManager* Instance();

	Tweakable* Find(const String& id) const;
};
} // namespace Debug
} // namespace LittleEngine
#endif