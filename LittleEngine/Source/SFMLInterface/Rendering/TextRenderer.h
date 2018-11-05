#pragma once
#include <string>
#include "Renderer.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	enum class HAlign {
		Centre,
		Left,
		Right
	};

	enum class VAlign {
		Centre,
		Top,
		Bottom
	};

	// \brief Container for text rendering metadata
	struct TextData {
	public:
		Fixed pixelSize;
		Fixed outlineSize;
		std::string text;
		Colour fillColour;
		Colour outlineColour;
		HAlign hAlign = HAlign::Centre;
		VAlign vAlign = VAlign::Bottom;

		TextData(FontAsset& font, const std::string& text);
		TextData(FontAsset& font, const std::string& text, Fixed pixelSize, Colour fillColour);
		TextData(FontAsset& font, const std::string& text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour);

		void SetFont(FontAsset& font);

	private:
		friend class TextRenderer;
		FontAsset* font;

		float GetNAlignmentHorz() const;
		float GetNAlignmentVert() const;
	};

	// \brief Concrete wrapper for SFML text shape
	class TextRenderer : public Renderer {
	public:
		TextRenderer(const TextData& data);
		TextRenderer(const TextRenderer& prototype);
		virtual std::unique_ptr<Renderer> UClone() const override;

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
