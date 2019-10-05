#pragma once
#include "Core/CoreTypes.h"
#if ENABLED(TWEAKABLES)
#define TweakBool(id, address) LittleEngine::Debug::Tweakable id(#id, LittleEngine::Debug::TweakType::Bool, "false", address)
#define TweakS32(id, address) LittleEngine::Debug::Tweakable id(#id, LittleEngine::Debug::TweakType::S32, "-1", address)
#define TweakF32(id, address) LittleEngine::Debug::Tweakable id(#id, LittleEngine::Debug::TweakType::F32, "-1.0", address)
#define TweakString(id, address) LittleEngine::Debug::Tweakable id(#id, LittleEngine::Debug::TweakType::String, "", address)
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
enum class TweakType : u8
{
	String = 0,
	F32,
	S32,
	Bool,
};

class Tweakable final : private NoCopy
{
private:
	void* m_pTarget = nullptr;
	std::function<void(std::string_view)> m_callback;

public:
	std::string m_id;
	std::string m_value;
	TweakType m_type = TweakType::String;

public:
	Tweakable(std::string id, TweakType type, std::string value = "", void* pTarget = nullptr);
	~Tweakable();

	void Set(std::string stringValue);
	void Bind(void* pVar);
	void BindCallback(std::function<void(std::string_view)> callback);

private:
	void SyncString(std::string rawValue);
	void SyncF32(std::string rawValue);
	void SyncS32(std::string rawValue);
	void SyncBool(std::string rawValue);
};

class TweakManager
{
public:
	Map<std::string_view, Tweakable*> m_tweakables;

	static TweakManager* Instance();

	Tweakable* Find(std::string_view id) const;
};
} // namespace Debug
} // namespace LittleEngine
#endif
