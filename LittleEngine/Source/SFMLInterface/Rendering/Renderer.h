#pragma once
#include "Vector2.h"
#include "Rect2.h"
#include "Engine/Object.h"
#include "RenderParams.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine {
	using Rect2 = GameUtils::Rect2;

	// \brief Base class for all SFML Rendering
	class Renderer : public Object {
	public:
		LayerInfo layer;

		Renderer(std::string name);
		virtual ~Renderer();

		// Call this to render the entity using the passed RenderParams
		void Render(struct RenderParams& params);
		// Subclass will return its max Bounds in screen space
		virtual Rect2 GetBounds() const = 0;
		
		bool IsEnabled() const;
		void SetEnabled(bool enabled);
	
	protected:
		virtual void RenderInternal(struct RenderParams& params) = 0;
		virtual void SetPosition(const Vector2& screenPosition) = 0;
		virtual void SetRotation(const Fixed& screenRotation) = 0;

		sf::Vector2f Convert(const Vector2& vector);
		sf::Color Convert(const Colour& colour);
		Colour Convert(const sf::Color& colour);

	private:
		bool isEnabled = true;

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
	};
}
