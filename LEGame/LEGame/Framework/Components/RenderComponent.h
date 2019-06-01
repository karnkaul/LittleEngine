#pragma once
#include <optional>
#include "LEGame/Model/World/Component.h"

namespace LittleEngine
{
struct SpriteSheetData
{
	Vec<SFTexRect> frames;
	Vec<size_t> indices;

	SpriteSheetData() = default;
	SpriteSheetData(SFTexCoords frameBounds, u32 rows, u32 columns);
	SpriteSheetData(String serialised);
	SpriteSheetData(SpriteSheetData&&) = default;
	SpriteSheetData& operator=(SpriteSheetData&&) = default;

private:
	void Construct(SFTexCoords frameBounds, u32 rows, u32 columns);
	void Construct(String serialised);
};

class SpriteSheet
{
private:
	SpriteSheetData m_data;
	Vec<size_t>::const_iterator m_iterator;
	Time m_period;
	TextureAsset* m_pTexture = nullptr;
	u8 m_skipFrames = 0;
	bool m_bUseFrames = false;

public:
	SpriteSheet(SpriteSheetData data,
				TextureAsset* pTexture = nullptr,
				Time period = Time::Seconds(1.0f),
				bool bUseFrames = false,
				u8 skipFrames = 0);
	SpriteSheet(String id,
				Time period = Time::Seconds(1.0f),
				bool bUseFrames = false,
				u8 skipFrames = 0,
				String textureSuffix = ".png",
				String sheetSuffix = ".spritesheet");
	SpriteSheet(SpriteSheet&&) = default;
	SpriteSheet& operator=(SpriteSheet&&) = default;

private:
	SFTexRect GetFrame() const;
	void Next();

	friend class RenderComponent;
};

class RenderComponent : public AComponent
{
public:
	class SFPrimitive* m_pSFPrimitive = nullptr;
	std::optional<SpriteSheet> m_oSpriteSheet;
	Time m_flipElapsed;
	Time m_framePeriod;
	u8 m_flipSkippedFrames = 0;
	bool m_bFlippingSprites = false;

public:
	~RenderComponent() override;

	TimingType GetComponentTiming() const override;
	void OnCreated() override;
	void SetEnabled(bool bEnabled) override;
	void Tick(Time dt) override;

	RenderComponent* SetShape(LayerID layer);
	RenderComponent* SetSprite(TextureAsset& texture, LayerID layer);
	RenderComponent* SetSpriteSheet(SpriteSheet sheet, LayerID layer);
	RenderComponent* UnsetSpriteSheet();
	RenderComponent* SetSpriteFlip(bool bFlip);

private:
	void UpdatePrimitive(Time dt);
};
} // namespace LittleEngine
