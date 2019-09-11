#pragma once
#include <optional>
#include "Model/World/Component.h"
#include "Engine/Renderer/LEShaders.h"

namespace LittleEngine
{
struct SpriteSheetData
{
	Vec<Rect2> frames;
	Vec<size_t> indices;

	SpriteSheetData() = default;
	SpriteSheetData(u32 rows, u32 columns);
	SpriteSheetData(String serialised);
	SpriteSheetData(SpriteSheetData&&) = default;
	SpriteSheetData& operator=(SpriteSheetData&&) = default;

private:
	void Construct(u32 rows, u32 columns);
	void Construct(String serialised);
};

class SpriteSheet
{
private:
	SpriteSheetData m_data;
	Vector2 m_frameSize;
	Vec<size_t>::const_iterator m_iterator;
	Time m_period;
	TextureAsset* m_pTexture = nullptr;
	u8 m_skipFrames = 0;
	bool m_bUseFrames = false;

public:
	SpriteSheet(SpriteSheetData data, TextureAsset* pTexture = nullptr, Time period = Time::Seconds(1.0f), bool bUseFrames = false,
				u8 skipFrames = 0);
	SpriteSheet(String id, Time period = Time::Seconds(1.0f), bool bUseFrames = false, u8 skipFrames = 0, String textureSuffix = ".png",
				String sheetSuffix = ".spritesheet");
	SpriteSheet(SpriteSheet&&) = default;
	SpriteSheet& operator=(SpriteSheet&&) = default;

private:
	Rect2 Frame() const;
	Vector2 FrameSize() const;
	void Next();

	friend class RenderComponent;
};

class RenderComponent : public AComponent
{
public:
	class APrimitive* m_pPrimitive = nullptr;
	std::optional<SpriteSheet> m_oSpriteSheet;
	Time m_flipElapsed;
	Time m_framePeriod;
	u8 m_flipSkippedFrames = 0;
	bool m_bFlippingSprites = false;

public:
	RenderComponent();
	~RenderComponent() override;

protected:
	void OnCreated() override;

public:
	void Tick(Time dt) override;

public:
	TimingType Timing() const override;
	void SetEnabled(bool bEnabled) override;

public:
	class SFRect* SetRectangle(LayerID layer);
	class SFCircle* SetCircle(LayerID layer);
	class Quad* SetSprite(TextureAsset& texture, LayerID layer);
	class SFText* SetText(LayerID layer);
	RenderComponent* SetSpriteSheet(SpriteSheet sheet, LayerID layer);
	RenderComponent* UnsetSpriteSheet();
	RenderComponent* SetSpriteFlip(bool bFlip);
	RenderComponent* SetShader(Shader* pShader);
	template <typename T>
	RenderComponent* SetShader(const String& id);

	void ReconcilePrimitive();
	void UpdatePrimitive(Time dt = Time::Zero);
};

template <typename T>
RenderComponent* RenderComponent::SetShader(const String& id)
{
	auto pShader = g_pShaders->GetShader<T>(id);
	if (!pShader)
	{
		LOG_W("%s Shader %s not in ShaderRepository!", m_logName.c_str(), id.c_str());
	}
	else
	{
		SetShader(pShader);
	}
	return this;
}
} // namespace LittleEngine
