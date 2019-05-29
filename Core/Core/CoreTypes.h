#pragma once
#include <utility>
#include "Asserts.h"
#include "Deferred.h"
#include "Fixed.h"
#include "FileRW.h"
#include "Rect2.h"
#include "SimpleTime.h"
#include "StdTypes.h"
#include "UByte.h"
#include "Vector2.h"
#include "Transform.h"

template <typename T>
using Deferred = Core::Deferred<T>;
using Fixed = Core::Fixed;
using FileRW = Core::FileRW;
using Rect2 = Core::Rect2;
using Transform = Core::Transform;
using Time = Core::Time;
using UByte = Core::UByte;
using Vector2 = Core::Vector2;
