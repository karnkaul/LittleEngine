#include "ConsoleRenderer.h"
#if ENABLED(CONSOLE)
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Repository/LERepository.h"
#include "Model/UI/UIElement.h"
#include "LogLine.h"

namespace LittleEngine::Debug
{
ConsoleRenderer::ConsoleRenderer(LEContext& context) : m_textSize(LogLine::TEXT_SIZE)
{
	m_pFont = g_pRepository->DefaultFont();
	LayerID textLayer = static_cast<LayerID>(ToS32(LayerID::Max) - 2);

	// BG
	m_uBG = MakeUnique<UIElement>(static_cast<LayerID>(ToS32(textLayer) - 5), true);
	m_uBG->OnCreate(context, "ConsoleBG");
	m_uBG->SetPanel(m_bgColour);
	Vector2 bgSize = m_uBG->RectSize();
	bgSize.y *= Fixed::OneThird;
	m_uBG->SetRectSize(bgSize);
	m_uBG->m_transform.padding = {0, -Fixed::OneHalf * bgSize.y};
	m_uBG->m_transform.nPosition = {0, 1};
	minPadding = m_uBG->m_transform.padding;
	maxPadding = -minPadding;
	maxPadding.y *= Fixed(1.1f);
	m_uBG->m_transform.padding = maxPadding;
	m_uBG->Tick();

	// Separator
	m_uSeparator = MakeUnique<UIElement>(static_cast<LayerID>(ToS32(textLayer) - 3), true);
	m_uSeparator->OnCreate(context, "ConsoleSeparator", &m_uBG->m_transform);
	m_uSeparator->SetPanel(Colour(255, 255, 255, 150));
	m_uSeparator->SetRectSize({bgSize.x, 2});
	m_uSeparator->m_transform.nPosition = {0, -1};
	m_uSeparator->m_transform.padding = {0, m_textSize + 5};
	m_uSeparator->Tick();

	// Carat
	m_uCarat = MakeUnique<UIElement>(textLayer, true);
	m_uCarat->OnCreate(context, "ConsoleCarat", &m_uBG->m_transform, g_pRepository->DefaultFont());
	m_uCarat->m_transform.anchor = {-1, 1};
	m_uCarat->m_transform.nPosition = {-1, -1};
	u16 textPad = static_cast<u16>(m_textSize * 1.3f);
	m_uCarat->m_transform.padding = {0, textPad};
	m_uCarat->SetText(UIText(">", m_textSize, m_liveTextColour));
	m_uCarat->Tick();

	// Live Line
	m_uLiveText = MakeUnique<UIElement>(textLayer, true);
	m_uLiveText->OnCreate(context, "ConsoleLiveText", &m_uBG->m_transform, g_pRepository->DefaultFont());
	m_uLiveText->m_transform.SetParent(m_uBG->m_transform);
	m_uLiveText->m_transform.anchor = {-1, 1};
	m_uLiveText->m_transform.nPosition = {-Fixed(0.99f), -1};
	m_uLiveText->m_transform.padding = {0, textPad};
	m_uLiveText->Tick();

	// Cursor
	m_uCursor = MakeUnique<UIElement>(textLayer, true);
	m_uCursor->OnCreate(context, "ConsoleCursor", &m_uBG->m_transform, g_pRepository->DefaultFont());
	m_uCursor->SetText(UIText("|", m_textSize, m_liveTextColour));
	m_uCursor->Tick();

	// Log Lines
	u16 iPad = textPad + 2;
	m_logLinesCount = (m_uBG->RectSize().y.ToU32() / textPad) - 1;
	for (size_t i = 0; i < ToIdx(m_logLinesCount); ++i)
	{
		UPtr<UIElement> uLogLineI = MakeUnique<UIElement>(textLayer, true);
		uLogLineI->OnCreate(context, "LogLine" + Strings::ToString(i), &m_uBG->m_transform, g_pRepository->DefaultFont());
		uLogLineI->m_transform.anchor = {-1, 1};
		uLogLineI->m_transform.nPosition = {-1, -1};
		iPad += textPad;
		uLogLineI->m_transform.padding = {0, iPad};
		uLogLineI->Tick();
		m_uLogTexts.emplace_back(std::move(uLogLineI));
	}
}

ConsoleRenderer::~ConsoleRenderer() = default;

void ConsoleRenderer::Tick(Time dt)
{
	Fixed dy(dt.AsMilliseconds());
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

	Vector2 cursorPosition = m_uLiveText->Text()->Position();
	Fixed xOffset = m_cursorNPos * m_uLiveText->Text()->RenderBounds().Size().x;
	cursorPosition.x += xOffset;
	cursorPosition.y -= m_textSize / 2;
	m_uCursor->Text()->SetPosition(cursorPosition);
	for (auto& uLogText : m_uLogTexts)
	{
		uLogText->Tick(dt);
	}
}

void ConsoleRenderer::SetLiveString(String text, Fixed cursorNPos, bool bShowCursor)
{
	m_cursorNPos = cursorNPos;
	m_uLiveText->SetText(UIText(std::move(text), m_textSize, m_liveTextColour));
	m_uCursor->Text()->SetEnabled(bShowCursor);
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
} // namespace LittleEngine::Debug
#endif
