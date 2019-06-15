#include "stdafx.h"
#include "Core/ArchiveReader.h"
#include "Core/GData.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LEGame/Model/World/Entity.h"
#include "LEGame/Model/GameManager.h"
#include "LittleEngine/Repository/LERepository.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "RenderComponent.h"

namespace LittleEngine
{
SpriteSheetData::SpriteSheetData(u32 rows, u32 columns)
{
	Construct(rows, columns);
}

SpriteSheetData::SpriteSheetData(String serialised)
{
	Construct(std::move(serialised));
}

void SpriteSheetData::Construct(u32 rows, u32 columns)
{
	size_t idx = 0;
	for (u32 row = 0; row < rows; ++row)
	{
		for (u32 column = 0; column < columns; ++column)
		{
			Fixed u(static_cast<f32>(column) / columns);
			Fixed v(static_cast<f32>(row) / rows);
			Fixed du(static_cast<f32>(column + 1) / columns);
			Fixed dv(static_cast<f32>(row + 1) / rows);
			Vector2 tl(u, v);
			Vector2 br(du , dv);
			frames.emplace_back(Rect2::TLBR(tl, br));
			indices.push_back(idx++);
		}
	}
}

void SpriteSheetData::Construct(String serialised)
{
	Core::GData gData(std::move(serialised));
	u32 rows = static_cast<u32>(gData.GetS32("rows", 0));
	u32 columns = static_cast<u32>(gData.GetS32("columns", 0));
	Construct(rows, columns);
}

SpriteSheet::SpriteSheet(SpriteSheetData data, TextureAsset* pTexture, Time period, bool bUseFrames, u8 skipFrames)
	: m_data(std::move(data)), m_period(period), m_pTexture(pTexture), m_skipFrames(skipFrames), m_bUseFrames(bUseFrames)
{
	m_iterator = m_data.indices.begin();
}

SpriteSheet::SpriteSheet(String id,
						 Time period /*= Time::Seconds(1.0f)*/,
						 bool bUseFrames /*= false*/,
						 u8 skipFrames /*= 0*/,
						 String textureSuffix /*= ".png"*/,
						 String sheetSuffix /*= ".spritesheet"*/)
	: m_period(period), m_skipFrames(skipFrames), m_bUseFrames(bUseFrames)
{
	auto pText = g_pRepository->Load<TextAsset>(id + sheetSuffix);
	auto pTexture = g_pRepository->Load<TextureAsset>(id + textureSuffix);
	if (pTexture && pText)
	{
		m_pTexture = pTexture;
		m_data = SpriteSheetData(pText->GetText());
		if (!m_data.frames.empty())
		{
			m_frameSize = m_data.frames[0].GetSize();
			Vector2 textureSize = pTexture->GetTextureSize();
			m_frameSize.x *= textureSize.x;
			m_frameSize.y *= textureSize.y;
		}
		m_iterator = m_data.indices.begin();
	}
	else
	{
		LOG_W("Error loading SpriteSheet: %s", id.c_str());
	}
}

Rect2 SpriteSheet::GetFrame() const
{
	if (!m_data.frames.empty())
	{
		size_t idx = *m_iterator;
		if (idx >= 0 && idx < m_data.frames.size())
		{
			return m_data.frames[idx];
		}
	}
	return {};
}

Vector2 SpriteSheet::GetFrameSize() const
{
	return m_frameSize;
}

void SpriteSheet::Next()
{
	++m_iterator;
	if (m_iterator == m_data.indices.end())
	{
		m_iterator = m_data.indices.begin();
	}
}

RenderComponent::~RenderComponent()
{
	if (m_pPrimitive)
	{
		m_pPrimitive->Destroy();
	}
}

TimingType RenderComponent::GetComponentTiming() const
{
	return TimingType::POST;
}

void RenderComponent::OnCreated()
{
	SetName("Render");
}

void RenderComponent::SetEnabled(bool bEnabled)
{
	Super::SetEnabled(bEnabled);

	if (m_pPrimitive)
	{
		m_pPrimitive->SetEnabled(bEnabled);
	}
}

void RenderComponent::Tick(Time dt)
{
	UpdatePrimitive(dt);
}

SFRect* RenderComponent::SetRectangle(LayerID layer)
{
	if (m_pPrimitive)
	{
		m_pPrimitive->Destroy();
	}
	auto pRect = g_pGameManager->Renderer()->New<SFRect>(layer);
	Assert(pRect, "Could not provision Primitive!");
	pRect->SetEnabled(true);
	m_pPrimitive = pRect;
	return pRect;
}

SFCircle* RenderComponent::SetCircle(LayerID layer)
{
	if (m_pPrimitive)
	{
		m_pPrimitive->Destroy();
	}
	auto pCircle = g_pGameManager->Renderer()->New<SFCircle>(layer);
	Assert(pCircle, "Could not provision Primitive!");
	pCircle->SetEnabled(true);
	m_pPrimitive = pCircle;
	return pCircle;
}

Quad* RenderComponent::SetSprite(TextureAsset& texture, LayerID layer)
{
	if (m_pPrimitive)
	{
		m_pPrimitive->Destroy();
	}
	auto pQuad = g_pGameManager->Renderer()->New<Quad>(layer);
	Assert(pQuad, "Could not provision Primitive!");
	pQuad->SetModel(Rect2::CentreSize(texture.GetTextureSize()))->SetTexture(texture)->SetEnabled(true);
	m_pPrimitive = pQuad;
	return pQuad;
}

SFText* RenderComponent::SetText(LayerID layer)
{
	if (m_pPrimitive)
	{
		m_pPrimitive->Destroy();
	}
	auto pText = g_pGameManager->Renderer()->New<SFText>(layer);
	Assert(pText, "Could not provision Primitive!");
	pText->SetEnabled(true);
	m_pPrimitive = pText;
	return pText;
}

RenderComponent* RenderComponent::SetSpriteSheet(SpriteSheet sheet, LayerID layer)
{
	if (sheet.m_pTexture)
	{
		if (m_pPrimitive)
		{
			m_pPrimitive->Destroy();
		}
		auto pQuad = g_pGameManager->Renderer()->New<Quad>(layer);
		Assert(pQuad, "Could not provision Primitive!");
		Rect2 model = Rect2::CentreSize(sheet.GetFrameSize());
		pQuad->SetTexture(*sheet.m_pTexture)->SetModel(model)->SetEnabled(true);
		m_framePeriod = Time::Seconds(sheet.m_period.AsSeconds() / sheet.m_data.indices.size());
		m_oSpriteSheet.emplace(std::move(sheet));
		m_bFlippingSprites = true;
		LOG_D("Set up SpriteSheet on %s", LogNameStr());
		m_pPrimitive = pQuad;
	}
	return this;
}

RenderComponent* RenderComponent::UnsetSpriteSheet()
{
	m_oSpriteSheet.reset();
	m_bFlippingSprites = false;
	LOG_D("Unset SpriteSheet on %s", LogNameStr());
	return this;
}

RenderComponent* RenderComponent::SetSpriteFlip(bool bFlip)
{
	m_bFlippingSprites = bFlip;
	return this;
}

RenderComponent* RenderComponent::SetShader(SFShader* pShader)
{
#if ENABLED(DEBUG_LOGGING)
	if (pShader)
	{
		const char* szType = g_szShaderTypes[static_cast<size_t>(pShader->GetType())];
		LOG_D("%s %s %s Shader set", m_logName.c_str(), pShader->GetID().c_str(), szType);
	}
	else
	{
		LOG_D("%s Shader unset", m_logName.c_str());
	}
#endif

	m_pPrimitive->SetShader(pShader);
	return this;
}

void RenderComponent::UpdatePrimitive(Time dt)
{
	if (m_bFlippingSprites && m_oSpriteSheet)
	{
		if (m_oSpriteSheet->m_bUseFrames)
		{
			++m_flipSkippedFrames;
			if (m_flipSkippedFrames > m_oSpriteSheet->m_skipFrames)
			{
				m_flipSkippedFrames = 0;
				m_oSpriteSheet->Next();
			}
		}
		else
		{
			m_flipElapsed += dt;
			if (m_flipElapsed >= m_framePeriod)
			{
				m_flipElapsed = Time::Zero;
				m_oSpriteSheet->Next();
			}
		}
		if (m_pPrimitive)
		{
			auto pQuad = m_pPrimitive->CastTo<Quad>();
			pQuad->SetUV(m_oSpriteSheet->GetFrame(), true);
		}
	}

	if (m_pPrimitive)
	{
		m_pPrimitive->SetScale(m_pOwner->m_transform.Scale(), m_pOwner->m_bResetRenderState);
		m_pPrimitive->SetOrientation(m_pOwner->m_transform.Orientation(), m_pOwner->m_bResetRenderState);
		m_pPrimitive->SetPosition(m_pOwner->m_transform.Position(), m_pOwner->m_bResetRenderState);
		m_pOwner->m_bResetRenderState = false;
	}
}
} // namespace LittleEngine
