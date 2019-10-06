#include "ConsoleRenderer.h"
#if ENABLED(CONSOLE)
#include "Core/LECoreGame/LECoreUtils/Utils.h"
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
	m_pFont = g_pDefaultFont;
	LayerID textLayer = static_cast<LayerID>(ToS32(LayerID::Max) - 2);

	// BG
	m_uBG = std::make_unique<UIElement>(static_cast<LayerID>(ToS32(textLayer) - 5), true);
	m_uBG->OnCreate(context, "ConsoleBG");
	m_uBG->SetPanel(m_bgColour);
	Vector2 bgSize = m_uBG->RectSize();
	bgSize.y *= Fixed::OneThird;
	m_uBG->SetRectSize(bgSize);
	m_uBG->m_transform.posDelta = {0, -Fixed::OneHalf * bgSize.y};
	m_uBG->m_transform.nPosition = {0, 1};
	minPadding = m_uBG->m_transform.posDelta;
	maxPadding = -minPadding;
	maxPadding.y *= Fixed(1.1f);
	m_uBG->m_transform.posDelta = maxPadding;
	m_uBG->Tick();

	// Separator
	m_uSeparator = std::make_unique<UIElement>(static_cast<LayerID>(ToS32(textLayer) - 3), true);
	m_uSeparator->OnCreate(context, "ConsoleSeparator", &m_uBG->m_transform);
	m_uSeparator->SetPanel(Colour(255, 255, 255, 150));
	m_uSeparator->SetRectSize({bgSize.x, 2});
	m_uSeparator->m_transform.nPosition = {0, -1};
	m_uSeparator->m_transform.posDelta = {0, m_textSize + 5};
	m_uSeparator->Tick();

	// Carat
	m_uCarat = std::make_unique<UIElement>(textLayer, true);
	m_uCarat->OnCreate(context, "ConsoleCarat", &m_uBG->m_transform, g_pDefaultFont);
	m_uCarat->m_transform.anchor = {-1, 1};
	m_uCarat->m_transform.nPosition = {-1, -1};
	u16 textPad = static_cast<u16>(m_textSize * 1.3f);
	m_uCarat->m_transform.posDelta = {0, textPad};
	m_uCarat->SetText(UIText(">", m_textSize, m_liveTextColour));
	m_uCarat->Tick();

	// Live Line
	m_uLiveText = std::make_unique<UIElement>(textLayer, true);
	m_uLiveText->OnCreate(context, "ConsoleLiveText", &m_uBG->m_transform, g_pDefaultFont);
	m_uLiveText->m_transform.SetParent(m_uBG->m_transform);
	m_uLiveText->m_transform.anchor = {-1, 1};
	m_uLiveText->m_transform.nPosition = {-Fixed(0.99f), -1};
	m_uLiveText->m_transform.posDelta = {0, textPad};
	m_uLiveText->Tick();

	// Cursor
	m_uCursor = std::make_unique<UIElement>(textLayer, true);
	m_uCursor->OnCreate(context, "ConsoleCursor", &m_uBG->m_transform, g_pDefaultFont);
	m_uCursor->SetText(UIText("|", m_textSize, m_liveTextColour));
	m_uCursor->Tick();

	// Log Lines
	u16 iPad = textPad + 2;
	m_logLinesCount = (m_uBG->RectSize().y.ToU32() / textPad) - 1;
	for (size_t i = 0; i < ToIdx(m_logLinesCount); ++i)
	{
		UPtr<UIElement> uLogLineI = std::make_unique<UIElement>(textLayer, true);
		uLogLineI->OnCreate(context, "LogLine" + Strings::ToString(i), &m_uBG->m_transform, g_pDefaultFont);
		uLogLineI->m_transform.anchor = {-1, 1};
		uLogLineI->m_transform.nPosition = {-1, -1};
		iPad += textPad;
		uLogLineI->m_transform.posDelta = {0, iPad};
		uLogLineI->Tick();
		m_uLogTexts.emplace_back(std::move(uLogLineI));
	}
}

ConsoleRenderer::~ConsoleRenderer() = default;

void ConsoleRenderer::Tick(Time dt)
{
	Fixed dy(dt.AsMilliseconds() * 2);
	Vector2& p = m_uBG->m_transform.posDelta;
	if (Console::g_bEnabled)
	{
		p.y = (p.y - dy > minPadding.y) ? p.y - dy : (p.y < minPadding.y) ? minPadding.y : p.y;
	}
	if (!Console::g_bEnabled)
	{
		p.y = (p.y < maxPadding.y) ? p.y + dy : (p.y > maxPadding.y) ? p.y = maxPadding.y : p.y;
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

void ConsoleRenderer::SetLiveString(std::string text, Fixed cursorNPos, bool bShowCursor)
{
	m_cursorNPos = cursorNPos;
	m_uLiveText->SetText(UIText(std::move(text), m_textSize, m_liveTextColour));
	m_uCursor->Text()->SetEnabled(bShowCursor);
}

void ConsoleRenderer::UpdateLog(std::vector<LogLine> logLines)
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
