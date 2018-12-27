#pragma once
#include "UIElement.h"

namespace LittleEngine {
	class UIProgressBar : public UIElement {
	protected:
		Vector2 m_size;

	public:
		UIProgressBar(bool bSilent = false);
		UIProgressBar(const std::string& name, bool bSilent = false);
		virtual ~UIProgressBar();

		void InitProgressBar(const Vector2& size, Colour colour, const Fixed& initProgress = Fixed::Zero);
		void SetProgress(const Fixed& progress);
	};
}
