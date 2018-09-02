#pragma once
#include <string>
#include "Renderer.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	enum Style {

	};

	// \brief Container for text rendering metadata
	struct TextData {
	public:
		Fixed pixelSize;
		Fixed outlineSize;
		std::string text;
		Colour fillColour;
		Colour outlineColour;

		TextData(FontAsset::Ptr font, const std::string& text);
		TextData(FontAsset::Ptr font, const std::string& text, Fixed pixelSize, Colour fillColour);
		TextData(FontAsset::Ptr font, const std::string& text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour);

		void SetFont(FontAsset::Ptr font);

	private:
		friend class TextRenderer;
		FontAsset::Ptr font;
	};

	// \brief Concrete wrapper for SFML text shape
	class TextRenderer : public Renderer {
	public:
		TextRenderer(const TextData& data);

		TextData& GetTextData();
		TextRenderer& SetSize(const Fixed& pixelSize);
		TextRenderer& SetColour(const Colour& colour);

		virtual Rect2 GetBounds() const override;

	protected:
		void ApplyData();

		virtual void RenderInternal(struct RenderParams& params) override;
		virtual void SetPosition(const Vector2& screenPosition) override;
		virtual void SetRotation(const Fixed& screenRotation) override;

	private:
		TextData data;
		sf::Text text;
	};
}
