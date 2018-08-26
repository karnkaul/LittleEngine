#include "stdafx.h"
#include "Renderer.h"
#include "Engine/Logger/Logger.h"
#include "Engine/World.h"
#include "SFML/Graphics.hpp"

namespace Game {
	const Colour Colour::Black(0, 0, 0);
	const Colour Colour::White;
	const Colour Colour::Red(255, 0, 0);
	const Colour Colour::Green(0, 255, 0);
	const Colour Colour::Blue(0, 0, 255);
	const Colour Colour::Yellow(255, 255, 0);
	const Colour Colour::Magenta(255, 0, 255);
	const Colour Colour::Cyan(0, 255, 255);
	const Colour Colour::Transparent(0, 0, 0, 0);

	Renderer::Renderer(std::string name) : Object(name) {
		Logger::Log(*this, name + " created");
	}

	Renderer::~Renderer() {
		Logger::Log(*this, name + " destroyed");
	}

	sf::Vector2f Renderer::Convert(const Vector2& vector) {
		return sf::Vector2f(vector.x.GetFloat(), vector.y.GetFloat());
	}

	sf::Color Renderer::Convert(const Colour& colour) {
		return sf::Color(
			colour.r.GetInt(),
			colour.g.GetInt(),
			colour.b.GetInt(),
			colour.a.GetInt()
		);
	}

	Colour Renderer::Convert(const sf::Color& colour) {
		return Colour(colour.r, colour.g, colour.b, colour.a);
	}
}
