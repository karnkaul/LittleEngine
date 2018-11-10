#pragma once
#include "Engine/Object.h"
#include "Fixed.h"

namespace LittleEngine {
	class Actor;
	struct RenderParams;

	using Fixed = GameUtils::Fixed;

	// \brief Base class for any Component attached to Actor
	class Component : public Object {
	public:
		using Ptr = std::shared_ptr<Component>;

		bool bEnabled = true;

		Component() = delete;
		Component(Actor& owner, const Component& prototype);
		virtual ~Component();

		virtual void Tick(Fixed deltaTime);
		virtual void FixedTick();
		virtual void Render(RenderParams params);

		Actor& GetActor() const;

		// Copiable sub-classes must override and return a copy for the new owner
		virtual Component::Ptr SClone(Actor& owner) const;
		virtual std::string ToString() const override;

	protected:
		Component(Actor& actor, const std::string& name);
		
	private:
		Actor* actor;

		Component& operator=(Component&) = delete;
	};
}
