#pragma once
#include "Engine/Object.h"
#include "Entities/IWorldEntity.h"

namespace LittleEngine {
	class UIObject : public Object, public IWorldEntity {
	protected:
		bool m_bDestroyed = false;

	public:
		UIObject(const std::string& name);
		virtual ~UIObject();
	};
}
