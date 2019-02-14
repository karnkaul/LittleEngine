#pragma once
#include "Engine/CoreGame.hpp"

namespace LittleEngine {
	class IWorldEntity {
		virtual void Tick(const Fixed& deltaMS) = 0;
		virtual void Render() = 0;
	};
}
