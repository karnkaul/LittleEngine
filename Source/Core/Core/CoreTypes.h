#pragma once
#include <functional>
#include <mutex>
#include <utility>
#include "Asserts.h"
#include "Deferred.h"
#include "Fixed.h"
#include "FileRW.h"
#include "Matrix3.h"
#include "NoCopy.h"
#include "Rect2.h"
#include "SimpleTime.h"
#include "StdTypes.h"
#include "UByte.h"
#include "Vector2.h"
#include "Transform.h"

#define ENABLED(x) x

#if defined(DEBUGGING)
#define PROFILER 1
#define CONSOLE 1
#define RENDER_STATS 1
#define TWEAKABLES 1
#else
#define PROFILER 0
#define CONSOLE 0
#define RENDER_STATS 0
#define TWEAKABLES 0
#endif

#if defined(SHIPPING)
#define FILESYSTEM_ASSETS 0
#else
#define FILESYSTEM_ASSETS 1
#endif

namespace LittleEngine
{
using Lock = std::lock_guard<std::mutex>;
using Task = std::function<void()>;

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

using Token = SPtr<s32>;
using WToken = WPtr<s32>;

template <typename T>
using Deferred = Core::Deferred<T>;
using Fixed = Core::Fixed;
using FileRW = Core::FileRW;
using Matrix3 = Core::Matrix3;
using NoCopy = Core::NoCopy;
using Rect2 = Core::Rect2;
using Transform = Core::Transform;
using Time = Core::Time;
using UByte = Core::UByte;
using Vector2 = Core::Vector2;
using Vector3 = Core::Vector3;

template <typename T>
Token MakeToken(T value)
{
	return std::make_shared<s32>(ToS32(value));
}
} // namespace LittleEngine
