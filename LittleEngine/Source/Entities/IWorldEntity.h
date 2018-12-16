#pragma once
#include "Fixed.h"

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;

	class IWorldEntity {
		virtual void Tick(const Fixed& deltaMS) = 0;
		virtual void Render() = 0;
	};
}
