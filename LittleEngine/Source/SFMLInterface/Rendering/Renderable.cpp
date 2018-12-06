#include "le_stdafx.h"
#include "Renderable.h"
#include "Engine/Logger/Logger.h"
#include "SFML/Graphics.hpp"

namespace LittleEngine {
	const Colour Colour::Black(0, 0, 0);
	const Colour Colour::White;
	const Colour Colour::Red(255, 0, 0);
	const Colour Colour::Green(0, 255, 0);
	const Colour Colour::Blue(0, 0, 255);
	const Colour Colour::Yellow(255, 255, 0);
	const Colour Colour::Magenta(255, 0, 255);
	const Colour Colour::Cyan(0, 255, 255);
	const Colour Colour::Transparent(0, 0, 0, 0);

	Renderable::Renderable(std::string name, bool bSilent) : Object(name), m_bSilent(bSilent) {
		if (!bSilent) Logger::Log(*this, name + " created", Logger::Severity::Debug);
	}

	Renderable::Renderable(const Renderable& prototype, bool bSilent) : Object(prototype.m_name + "_clone"), m_layer(prototype.m_layer) {
		if (!bSilent) Logger::Log(*this, m_name + " cloned", Logger::Severity::Debug);
	}

	Renderable::~Renderable() {
		if (!m_bSilent) Logger::Log(*this, m_name + " destroyed", Logger::Severity::Debug);
	}

	void Renderable::Render(const RenderParams & params) {
		if (m_bIsEnabled) {
			SetPosition(params.GetScreenPosition());
			SetOrientation(params.GetScreenOrientation());
			SetScale(params.GetScreenScale());
			RenderInternal();
		}
	}

	bool Renderable::IsEnabled() const {
		return m_bIsEnabled;
	}

	void Renderable::SetEnabled(bool enabled) {
		m_bIsEnabled = enabled;
	}
}
