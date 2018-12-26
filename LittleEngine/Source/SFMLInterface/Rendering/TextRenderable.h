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
	private:
		std::string text;
		Fixed pixelSize;
		Fixed outlineSize;
		Colour fillColour;
		Colour outlineColour;
		HAlign hAlign = HAlign::Centre;
		VAlign vAlign = VAlign::Bottom;
		FontAsset* pFont;
		bool bCustomPivot = false;
		bool bDirty = false;

	public:
		TextData(FontAsset& font, const std::string& text);
		TextData(FontAsset& font, const std::string& text, Fixed pixelSize, Colour fillColour);
		TextData(FontAsset& font, const std::string& text, Fixed pixelSize, Colour fillColour, Fixed outlineSize, Colour outlineColour);

		void SetFont(FontAsset& font);
		void SetText(const std::string& text);
		void SetPixelSize(Fixed pixelSize);
		void SetFillColour(Colour fillColour);
		void SetOutlineColour(Colour outlineColour);
		void SetHAlign(HAlign hAlign);
		void SetVAlign(VAlign vAlign);

		const std::string& GetText() const;
		Fixed GetPixelSize() const;
		Colour GetFillColour() const;
		Colour GetOutlineColour() const;
		HAlign GetHAlign() const;
		VAlign GetVAlign() const;

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
		virtual void SetPivot(const Vector2& pivot) override;

	protected:
		void ApplyData();

		virtual void RenderInternal() override;
		virtual void SetPosition(const Vector2& screenPosition) override;
		virtual void SetOrientation(const Fixed& screenOrientation) override;
		virtual void SetScale(const Vector2& screenScale) override;
	};
}
