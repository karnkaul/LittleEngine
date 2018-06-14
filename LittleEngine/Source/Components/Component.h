#pragma once
#include "Engine/Object.h"
#include "Utils/Fixed.h"

namespace Game {
	class Actor;
	struct RenderParams;

	class Component : public Object {
	public:
		Component() = delete;
		virtual ~Component();
		virtual void Tick(Fixed deltaTime);
		virtual void FixedTick();
		virtual void Render(RenderParams& params);
		bool enabled = true;

	protected:
		Component(Actor& actor, const std::string& name);
		Actor& GetActor() const;
	
	private:
		Actor* actor;
		Component(const Component&) = delete;
		Component& operator=(Component&) = delete;
	};
}