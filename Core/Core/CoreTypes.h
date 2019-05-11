#pragma once
#include <utility>
#include "Core/Asserts.h"
#include "Core/Deferred.h"
#include "Core/Fixed.h"
#include "Core/FileRW.h"
#include "Core/Rect2.h"
#include "Core/SimpleTime.h"
#include "Core/StdTypes.h"
#include "Core/UByte.h"
#include "Core/Vector2.h"
#include "Core/Transform.h"

template <typename T>
using Deferred = Core::Deferred<T>;
using Fixed = Core::Fixed;
using FileRW = Core::FileRW;
using Rect2 = Core::Rect2;
using Transform = Core::Transform;
using Time = Core::Time;
using UByte = Core::UByte;
using Vector2 = Core::Vector2;
