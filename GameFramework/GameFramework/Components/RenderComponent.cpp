#include "stdafx.h"
#include "ArchiveReader.h"
#include "GData.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Game/Entity.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Services/Services.h"
#include "RenderComponent.h"

namespace LittleEngine
{
SpriteSheetData::SpriteSheetData(SFTexCoords frameBounds, u32 rows, u32 columns)
{
	Construct(frameBounds, rows, columns);
}

SpriteSheetData::SpriteSheetData(String serialised)
{
	Construct(std::move(serialised));
}

void SpriteSheetData::Construct(SFTexCoords frameBounds, u32 rows, u32 columns)
{
	size_t idx = 0;
	for (u32 row = 0; row < rows; ++row)
	{
		for (u32 column = 0; column < columns; ++column)
		{
			SFTexCoords min(column * frameBounds.x, row * frameBounds.y);
			SFTexCoords max(min.x + frameBounds.x, min.y + frameBounds.y);
			frames.emplace_back(min, max);
			indices.push_back(idx++);
		}
	}
}

void SpriteSheetData::Construct(String serialised)
{
	Core::GData gData(std::move(serialised));
	Vec<String> frameBoundsText = gData.GetVector("frameBounds");
	u32 rows = static_cast<u32>(gData.GetS32("rows", 0));
	u32 columns = static_cast<u32>(gData.GetS32("columns", 0));
	if (frameBoundsText.size() >= 2)
	{
		u32 min = static_cast<u32>(Strings::ToS32(frameBoundsText[0], 0));
		u32 max = static_cast<u32>(Strings::ToS32(frameBoundsText[1], 0));
		Construct(SFTexCoords(min, max), rows, columns);
	}
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
	auto pText = Services::Game()->Repository()->Load<TextAsset>(id + sheetSuffix);
	auto pTexture = Services::Game()->Repository()->Load<TextureAsset>(id + textureSuffix);
	if (pTexture && pText)
	{
		m_pTexture = pTexture;
		m_data = SpriteSheetData(pText->GetText());
		m_iterator = m_data.indices.begin();
	}
	else
	{
		LOG_W("Error loading SpriteSheet: %s", id.c_str());
	}
}

SFTexRect SpriteSheet::GetFrame() const
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
	Services::RHeap()->Destroy(m_pSFPrimitive);
}

TimingType RenderComponent::GetComponentTiming() const
{
	return TimingType::POST;
}

void RenderComponent::OnCreated()
{
	SetName("Render");
	m_pSFPrimitive = Services::RHeap()->New();
	m_pSFPrimitive->SetEnabled(true);
}

void RenderComponent::SetEnabled(bool bEnabled)
{
	AComponent::SetEnabled(bEnabled);

	m_pSFPrimitive->SetEnabled(bEnabled);
}

void RenderComponent::Tick(Time dt)
{
	UpdatePrimitive(dt);
}

RenderComponent* RenderComponent::SetSprite(TextureAsset& texture)
{
	m_pSFPrimitive->SetTexture(texture);
	return this;
}

RenderComponent* RenderComponent::SetSpriteSheet(SpriteSheet sheet)
{
	if (sheet.m_pTexture)
	{
		m_pSFPrimitive->SetTexture(*sheet.m_pTexture);
	}
	m_framePeriod = Time::Seconds(sheet.m_period.AsSeconds() / sheet.m_data.indices.size());
	m_oSpriteSheet.emplace(std::move(sheet));
	m_bFlippingSprites = true;
	LOG_D("Set up SpriteSheet on %s", LogNameStr());
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
		m_pSFPrimitive->CropTexture(m_oSpriteSheet->GetFrame());
	}

	m_pSFPrimitive->SetScale(m_pOwner->m_transform.Scale(), m_pOwner->m_bReset);
	m_pSFPrimitive->SetOrientation(m_pOwner->m_transform.Orientation(), m_pOwner->m_bReset);
	m_pSFPrimitive->SetPosition(m_pOwner->m_transform.Position(), m_pOwner->m_bReset);
	m_pOwner->m_bReset = false;
}
} // namespace LittleEngine
