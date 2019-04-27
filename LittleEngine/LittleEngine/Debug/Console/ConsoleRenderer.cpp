#include "stdafx.h"
#include "ConsoleRenderer.h"
#if ENABLED(CONSOLE)
#include "Logger.h"
#include "Utils.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIElement.h"
#include "LittleEngine/UI/UIText.h"
#include "LogLine.h"

namespace LittleEngine
{
namespace Debug
{
ConsoleRenderer::ConsoleRenderer() : m_textSize(LogLine::TEXT_SIZE)
{
	m_pFont = Services::Engine()->Repository()->GetDefaultFont();

	// BG
	m_uBG = MakeUnique<UIElement>("ConsoleBG", static_cast<LayerID>(LAYER_TOP - 5), true);
	m_uBG->SetPanel(m_bgColour);
	Vector2 bgSize = m_uBG->m_transform.size;
	bgSize.y *= Fixed::OneThird;
	m_uBG->m_transform.size = bgSize;
	m_uBG->m_transform.padding = {0, -Fixed::OneHalf * bgSize.y};
	m_uBG->m_transform.nPosition = {0, 1};
	minPadding = m_uBG->m_transform.padding;
	maxPadding = -minPadding;
	maxPadding.y *= Fixed(1.1f);
	m_uBG->m_transform.padding = maxPadding;

	// Separator
	m_uSeparator = MakeUnique<UIElement>("ConsoleSeparator", static_cast<LayerID>(LAYER_TOP - 3), true);
	m_uSeparator->SetPanel(Colour(255, 255, 255, 150));
	m_uSeparator->m_transform.SetParent(m_uBG->m_transform);
	m_uSeparator->m_transform.size = {bgSize.x, 2};
	m_uSeparator->m_transform.nPosition = {0, -1};
	m_uSeparator->m_transform.padding = {0, m_textSize + 5};
	
	// Carat
	m_uCarat = MakeUnique<UIElement>("ConsoleCarat", LAYER_TOP, true);
	m_uCarat->m_transform.SetParent(m_uBG->m_transform);
	m_uCarat->m_transform.anchor = {-1, 1};
	m_uCarat->m_transform.nPosition = {-1, -1};
	s32 textPad = m_textSize * 1.3f;
	m_uCarat->m_transform.padding = {0, textPad};
	m_uCarat->SetText(UIText(">", m_textSize, m_liveTextColour));
	
	// Live Line
	m_uLiveText = MakeUnique<UIElement>("ConsoleLiveText", LAYER_TOP, true);
	m_uLiveText->m_transform.SetParent(m_uBG->m_transform);
	m_uLiveText->m_transform.anchor = {-1, 1};
	m_uLiveText->m_transform.nPosition = {-Fixed(0.99f), -1};
	m_uLiveText->m_transform.padding = {0, textPad};
	
	// Cursor
	m_uCursor = MakeUnique<UIElement>("ConsoleCursor", LAYER_TOP, true);
	m_uCursor->SetText(UIText("|", m_textSize, m_liveTextColour));
	m_uCursor->GetText()->SetEnabled(true);
	
	// Log Lines
	s32 iPad = textPad + 2;
	m_logLinesCount = (m_uBG->m_transform.size.y.ToU32() / textPad) - 1;
	for (size_t i = 0; i < ToIdx(m_logLinesCount); ++i)
	{
		UPtr<UIElement> uLogLineI = MakeUnique<UIElement>("Log Line " + Strings::ToString(i), LAYER_TOP, true);
		uLogLineI->m_transform.SetParent(m_uBG->m_transform);
		uLogLineI->m_transform.anchor = {-1, 1};
		uLogLineI->m_transform.nPosition = {-1, -1};
		iPad += textPad;
		uLogLineI->m_transform.padding = {0, iPad};
		m_uLogTexts.emplace_back(std::move(uLogLineI));
	}
}

ConsoleRenderer::~ConsoleRenderer() = default;

void ConsoleRenderer::Tick(Time dt)
{
	Fixed dy(dt.AsMilliseconds() * 2);
	if (Console::g_bEnabled)
	{
		if (m_uBG->m_transform.padding.y > minPadding.y)
		{
			m_uBG->m_transform.padding.y -= dy;
		}
		if (m_uBG->m_transform.padding.y < minPadding.y)
		{
			m_uBG->m_transform.padding.y = minPadding.y;
		}
	}
	if (!Console::g_bEnabled)
	{
		if (m_uBG->m_transform.padding.y < maxPadding.y)
		{
			m_uBG->m_transform.padding.y += dy;
		}
		if (m_uBG->m_transform.padding.y > maxPadding.y)
		{
			m_uBG->m_transform.padding.y = maxPadding.y;
		}
	}
	m_uBG->Tick(dt);
	m_uSeparator->Tick(dt);
	m_uCarat->Tick(dt);
	m_uLiveText->Tick(dt);
	m_uCursor->Tick(dt);
	
	Vector2 cursorPosition = m_uLiveText->GetText()->GetPosition();
	Fixed xOffset = m_cursorNPos * m_uLiveText->GetText()->GetBounds().GetSize().x;
	cursorPosition.x += xOffset;
	cursorPosition.y -= m_textSize / 2;
	m_uCursor->GetText()->SetPosition(cursorPosition);
	for (auto& uLogText : m_uLogTexts)
	{
		uLogText->Tick(dt);
	}
}

void ConsoleRenderer::SetLiveString(String text, Fixed cursorNPos, bool bShowCursor)
{
	m_cursorNPos = cursorNPos;
	m_uLiveText->SetText(UIText(std::move(text), m_textSize, m_liveTextColour));
	m_uCursor->GetText()->SetEnabled(bShowCursor);
}

void ConsoleRenderer::UpdateLog(Vec<LogLine> logLines)
{
	auto iter = logLines.begin();
	for (auto& uLogText : m_uLogTexts)
	{
		if (iter == logLines.end())
		{
			break;
		}
		uLogText->SetText(iter->ToUIText());
		++iter;
	}
}
} // namespace Debug
} // namespace LittleEngine
#endif
