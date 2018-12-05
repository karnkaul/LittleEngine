#pragma once
#include <string>
#include "Renderable.h"
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
		std::string text;
		Fixed pixelSize;
		Fixed outlineSize;
		Colour fillColour;
		Colour outlineColour;
		HAlign hAlign = HAlign::Centre;
		VAlign vAlign = VAlign::Bottom;
	private:
		FontAsset* pFont;

	public:
		TextData(FontAsset& font, const std::string& text);
		TextData(FontAsset& font, const std::string& text, Fixed pixelSize, Colour fillColour);
		TextData(FontAsset& font, const std::string& text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour);

		void SetFont(FontAsset& font);

	private:
		float GetNAlignmentHorz() const;
		float GetNAlignmentVert() const;

		friend class TextRenderable;
	};

	// \brief Concrete wrapper for SFML text shape
	class TextRenderable : public Renderable {
	private:
		TextData m_data;
		sf::Text m_sfText;

	public:
		TextRenderable(const TextData& data);
		TextRenderable(const TextRenderable& prototype);
		virtual std::unique_ptr<Renderable> UClone() const override;

		TextData& GetTextData();
		TextRenderable& SetSize(const Fixed& pixelSize);
		TextRenderable& SetColour(const Colour& colour);

		virtual Rect2 GetBounds() const override;

	protected:
		void ApplyData();

		virtual void RenderInternal(struct RenderParams& params) override;
		virtual void SetPosition(const Vector2& screenPosition) override;
		virtual void SetRotation(const Fixed& screenRotation) override;
		virtual void SetScale(const Vector2& screenScale) override;
	};
}
