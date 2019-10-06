#pragma once
#include <cstdint>
#if __MINGW32__
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <type_traits>

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
using size_t = std::size_t;

template <typename Base, typename Derived>
constexpr bool IsDerived()
{
	return std::is_base_of<Base, Derived>::value;
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
