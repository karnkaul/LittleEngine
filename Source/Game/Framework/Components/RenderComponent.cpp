#include "Core/ArchiveReader.h"
#include "Core/GData.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "SFMLAPI/System/Assets.h"
#include "Model/World/Entity.h"
#include "Model/GameManager.h"
#include "Model/World/Camera.h"
#include "Engine/Repository/LERepository.h"
#include "Engine/Renderer/LERenderer.h"
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
			Vector2 br(du, dv);
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

SpriteSheet::SpriteSheet(String id, Time period /*= Time::Seconds(1.0f)*/, bool bUseFrames /*= false*/, u8 skipFrames /*= 0*/,
						 String textureSuffix /*= ".png"*/, String sheetSuffix /*= ".spritesheet"*/)
	: m_period(period), m_skipFrames(skipFrames), m_bUseFrames(bUseFrames)
{
	auto pText = g_pRepository->Load<TextAsset>(id + sheetSuffix);
	auto pTexture = g_pRepository->Load<TextureAsset>(id + textureSuffix);
	if (pTexture && pText)
	{
		m_pTexture = pTexture;
		m_data = SpriteSheetData(pText->Text());
		if (!m_data.frames.empty())
		{
			m_frameSize = m_data.frames[0].Size();
			Vector2 textureSize = pTexture->TextureSize();
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

Rect2 SpriteSheet::Frame() const
{
	if (!m_data.frames.empty())
	{
		size_t idx = *m_iterator;
		if (idx < m_data.frames.size())
		{
			return m_data.frames[idx];
		}
	}
	return {};
}

Vector2 SpriteSheet::FrameSize() const
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

RenderComponent::RenderComponent() = default;

RenderComponent::~RenderComponent()
{
	if (m_pPrimitive)
	{
		m_pPrimitive->Destroy();
	}
}

void RenderComponent::OnCreated()
{
	SetName("Render");
}

void RenderComponent::Tick(Time dt)
{
	UpdatePrimitive(dt);
}

TimingType RenderComponent::Timing() const
{
	return TimingType::Post;
}

void RenderComponent::SetEnabled(bool bEnabled)
{
	Super::SetEnabled(bEnabled);

	if (m_pPrimitive)
	{
		if (bEnabled)
		{
			UpdatePrimitive();
		}
		else
		{
			m_pOwner->m_bResetRenderState = true;
		}
		m_pPrimitive->SetEnabled(bEnabled);
	}
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
	pQuad->SetModel(Rect2::SizeCentre(texture.TextureSize()))->SetTexture(texture)->SetEnabled(true);
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
		Rect2 model = Rect2::SizeCentre(sheet.FrameSize());
		pQuad->SetTexture(*sheet.m_pTexture)->SetModel(model)->SetEnabled(true);
		m_framePeriod = Time::Seconds(sheet.m_period.AsSeconds() / sheet.m_data.indices.size());
		m_oSpriteSheet.emplace(std::move(sheet));
		m_bFlippingSprites = true;
		LOG_D("Set up SpriteSheet on %s", m_logName.c_str());
		m_pPrimitive = pQuad;
	}
	return this;
}

RenderComponent* RenderComponent::UnsetSpriteSheet()
{
	m_oSpriteSheet.reset();
	m_bFlippingSprites = false;
	LOG_D("Unset SpriteSheet on %s", m_logName.c_str());
	return this;
}

RenderComponent* RenderComponent::SetSpriteFlip(bool bFlip)
{
	m_bFlippingSprites = bFlip;
	return this;
}

RenderComponent* RenderComponent::SetShader(Shader* pShader)
{
#if ENABLED(DEBUG_LOGGING)
	if (pShader)
	{
		VString type = g_szShaderTypes[pShader->GetType()];
		LOG_D("%s %s %s Shader set", m_logName.c_str(), pShader->ID().c_str(), type.data());
	}
	else
	{
		LOG_D("%s Shader unset", m_logName.c_str());
	}
#endif

	m_pPrimitive->SetShader(pShader);
	return this;
}

void RenderComponent::ReconcilePrimitive()
{
	UpdatePrimitive();
	m_pPrimitive->ReconcileGameState();
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
			pQuad->SetUV(m_oSpriteSheet->Frame(), true);
		}
	}

	if (m_pPrimitive)
	{
		Matrix3 mat = m_pOwner->WorldMatrix();
		m_pPrimitive->SetScale(m_pOwner->m_transform.WorldScale(), m_pOwner->m_bResetRenderState);
		m_pPrimitive->SetOrientation(mat.Orientation(), m_pOwner->m_bResetRenderState);
		m_pPrimitive->SetPosition(mat.Position(), m_pOwner->m_bResetRenderState);
		m_pOwner->m_bResetRenderState = false;
	}
}
} // namespace LittleEngine
