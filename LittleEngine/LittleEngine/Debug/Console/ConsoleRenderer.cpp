#include "stdafx.h"
#include "ConsoleRenderer.h"
#if ENABLED(CONSOLE)
#include "LittleEngine/Services/Services.h"

namespace LittleEngine {
	namespace Debug {
		ConsoleRenderer::ConsoleRenderer() {
			m_pFont = Services::Engine()->Repository()->GetDefaultFont();

			// BG
			m_uBG = MakeUnique<UIElement>("Console BG");
			m_uBG->SetPanel(m_bgColour);
			Vector2 bgSize = m_uBG->m_transform.size;
			bgSize.y *= Fixed::OneThird;
			m_uBG->m_transform.size = bgSize;
			m_uBG->m_transform.SetAutoPadNPosition({ 0, 1 });
			minPadding = m_uBG->m_transform.padding;
			maxPadding = -minPadding;
			maxPadding.y *= Fixed(1.1f);
			m_uBG->m_transform.padding = maxPadding;

			// Separator
			m_uSeparator = MakeUnique<UIElement>("Console Separator");
			m_uSeparator->SetPanel(Colour(255, 255, 255, 150));
			m_uSeparator->m_transform.SetParent(m_uBG->m_transform);
			m_uSeparator->m_transform.size = { bgSize.x, 2 };
			m_uSeparator->m_transform.nPosition = { 0, -1 };
			m_uSeparator->m_transform.padding = { 0, m_textSize + 5 };

			// Live Line
			m_uLiveText = MakeUnique<UIElement>("Console LiveText");
			m_uLiveText->m_transform.SetParent(m_uBG->m_transform);
			m_uLiveText->m_transform.anchor = { -1, 1 };
			m_uLiveText->m_transform.nPosition = { -1, -1 };
			s32 textPad = m_textSize * 1.3f;
			m_uLiveText->m_transform.padding = { 0, textPad };
			m_uLiveText->SetText(UIText(">", m_textSize, m_liveTextColour));

			// Log Lines
			s32 iPad = textPad + 2;
			m_logLinesCount = (m_uBG->m_transform.size.y.ToU32() / textPad) - 1;
			for (size_t i = 0; i < static_cast<size_t>(m_logLinesCount); ++i) {
				UPtr<UIElement> uLogLineI = MakeUnique<UIElement>("Log Line " + Strings::ToString(i));
				uLogLineI->m_transform.SetParent(m_uBG->m_transform);
				uLogLineI->m_transform.anchor = { -1, 1 };
				uLogLineI->m_transform.nPosition = { -1, -1 };
				iPad += textPad;
				uLogLineI->m_transform.padding = { 0, iPad };
				m_uLogTexts.emplace_back(std::move(uLogLineI));
			}
		}

		void ConsoleRenderer::Tick(Time dt) {
			Fixed dy(dt.AsMilliseconds() * 2);
			if (Console::g_bEnabled) {
				if (m_uBG->m_transform.padding.y > minPadding.y) m_uBG->m_transform.padding.y -= dy;
				if (m_uBG->m_transform.padding.y < minPadding.y) m_uBG->m_transform.padding.y = minPadding.y;
			}
			if (!Console::g_bEnabled) {
				if (m_uBG->m_transform.padding.y < maxPadding.y) m_uBG->m_transform.padding.y += dy;
				if (m_uBG->m_transform.padding.y > maxPadding.y) m_uBG->m_transform.padding.y = maxPadding.y;
			}
			m_uBG->Tick(dt);
			m_uSeparator->Tick(dt);
			m_uLiveText->Tick(dt);
			for (auto& uLogText : m_uLogTexts) {
				uLogText->Tick(dt);
			}
		}

		void ConsoleRenderer::SetLiveString(const String& text) {
			m_uLiveText->SetText(UIText(text, m_textSize, m_liveTextColour));
		}

		void ConsoleRenderer::UpdateLog(const Vector<LogLine>& logLines) {
			auto iter = logLines.begin();
			for (auto& uLogText : m_uLogTexts) {
				if (iter == logLines.end()) break;
				uLogText->SetText(iter->ToUIText());
				++iter;
			}
		}
	}
}
#endif
