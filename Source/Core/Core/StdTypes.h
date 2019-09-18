#pragma once
#include <cstdint>
#if __MINGW32__
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <array>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define ENABLED(x) x

#if defined(DEBUGGING)
#define PROFILER 1
#define ASSERTS 1
#define CONSOLE 1
#define RENDER_STATS 1
#define TWEAKABLES 1
#else
#define PROFILER 0
#define ASSERTS 0
#define CONSOLE 0
#define RENDER_STATS 0
#define TWEAKABLES 0
#endif

#if defined(SHIPPING)
#define DEBUG_LOGGING 0
#define FILESYSTEM_ASSETS 0
#else
#define DEBUG_LOGGING 1
#define FILESYSTEM_ASSETS 1
#endif

#if _MSC_VER
#define SPRINTF(szData, size, szFormat, ...) sprintf_s(szData, size, szFormat, ##__VA_ARGS__)
#define STRCAT(szData, size, szText) strcat_s(szData, size, szText)
#else
#define SPRINTF(szData, size, szFormat, ...) std::sprintf(szData, szFormat, ##__VA_ARGS__)
#define STRCAT(szData, size, szText) strcat(szData, szText)
#endif

using u8 = uint8_t;
using s8 = int8_t;
using u16 = uint16_t;
using s16 = int16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s32 = int32_t;
using s64 = int64_t;
using f32 = float;
using f64 = double;

using String = std::string;
using StringStream = std::stringstream;
using Task = std::function<void()>;
using Lock = std::lock_guard<std::mutex>;

template <typename T>
using InitList = std::initializer_list<T>;
template <typename T, size_t U>
using Array = std::array<T, U>;
template <typename T>
using Vec = std::vector<T>;
template <typename T>
using List = std::list<T>;
template <typename T>
using Set = std::set<T>;
template <typename T>
using USet = std::unordered_set<T>;
template <typename T, typename U>
using Map = std::map<T, U>;
template <typename T, typename U>
using UMap = std::unordered_map<T, U>;
template <typename T>
using UPtr = std::unique_ptr<T>;
template <typename T>
using SPtr = std::shared_ptr<T>;
template <typename T>
using WPtr = std::weak_ptr<T>;
template <typename T>
using Dual = std::pair<T, T>;
template <typename T, typename U>
using Pair = std::pair<T, U>;

template <typename Base, typename Derived>
constexpr bool IsDerived()
{
	return std::is_base_of<Base, Derived>::value;
}

template <typename T, typename... U>
UPtr<T> MakeUnique(U&&... Args)
{
	return std::make_unique<T>(std::forward<U>(Args)...);
}

template <typename T, typename... U>
SPtr<T> MakeShared(U&&... Args)
{
	return std::make_shared<T>(std::forward<U>(Args)...);
}

template <typename T>
constexpr size_t ToIdx(T t)
{
	return static_cast<size_t>(t);
}

template <typename T>
constexpr s32 ToS32(T t)
{
	return static_cast<s32>(t);
}

class DependencyException : public std::exception
{
};
