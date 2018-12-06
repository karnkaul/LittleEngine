#pragma once
#include "Vector2.h"
#include "Rect2.h"
#include "Engine/Object.h"
#include "RenderParams.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine {
	using Rect2 = GameUtils::Rect2;

	// \brief Base class for all SFML Rendering
	class Renderable : public Object {
	public:
		LayerInfo m_layer;
	private:
		bool m_bIsEnabled = true;
		bool m_bSilent;

	public:
		Renderable(std::string name, bool bSilent = false);
		Renderable(const Renderable& prototype, bool bSilent = false);
		virtual ~Renderable();

		// Call this to render the entity using the passed RenderParams
		void Render(const struct RenderParams& params);
		// Subclass will return its max Bounds in screen space
		virtual Rect2 GetBounds() const = 0;
		// Subclass will return an UNOWNED copy of itself
		virtual std::unique_ptr<Renderable> UClone() const = 0;
		
		bool IsEnabled() const;
		void SetEnabled(bool bEnabled);
	
	protected:
		virtual void RenderInternal() = 0;
		virtual void SetPosition(const Vector2& worldPosition) = 0;
		virtual void SetOrientation(const Fixed& worldOrientation) = 0;
		virtual void SetScale(const Vector2& worldScale) = 0;

	private:
		Renderable& operator=(const Renderable&) = delete;
	};
}
