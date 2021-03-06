#pragma once
#include <functional>
#include <mutex>
#include <utility>
#include "Game/Delegate/Delegate.h"
#include "Game/GameTypes/Fixed.h"
#include "Game/GameTypes/Matrix3.h"
#include "Game/GameTypes/Rect2.h"
#include "Game/GameTypes/SimpleTime.h"
#include "Game/GameTypes/Transform.h"
#include "Game/GameTypes/UByte.h"
#include "Game/GameTypes/Utils.h"
#include "Game/GameTypes/Vector2.h"
#include "Game/Deferred.h"
#include "Game/FileRW.h"
#include "Game/LEMacros.h"
#include "Game/NoCopy.h"
#include "Game/TRange.h"

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

using Token = std::shared_ptr<s32>;
using WToken = std::weak_ptr<s32>;

template <typename T>
using Deferred = Core::Deferred<T>;
template <typename T>
using TRange = Core::TRange<T>;
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
