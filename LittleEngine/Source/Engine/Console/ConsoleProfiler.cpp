#include "le_stdafx.h"
#include <vector>
#include "ConsoleProfiler.h"
#include "Engine/Engine.h"
#include "Engine/CoreGame.hpp"
#include "UI/UIProgressBar.h"
#include "SFMLInterface/Graphics.h"
#include "SFMLInterface/SystemClock.h"

namespace LittleEngine { namespace DebugConsole {
	namespace Profiler {
#if ENABLED(DEBUG_PROFILER)
#pragma region Local
		namespace {
			struct Entry {
				UIProgressBar progressBar;
				UIElement labelElement;
				std::string id;
				Colour colour;
				int64_t startUS = 0;
				int64_t endUS = 0;
				
				Entry(const std::string& id, Colour colour, int64_t startUS) : progressBar(UIProgressBar(true)), labelElement(UIElement(true)), id(id), colour(colour), startUS(startUS) {}
				int64_t ElapsedUS() const { return endUS - startUS; }
			};
			using UEntry = std::unique_ptr<Entry>;

			Vector2 progressBarSize;
			Fixed textWidth;
			Fixed profilerHeight;
			int64_t maxDeltaUS;
			int numEntries = 20;
			Engine* pEngine = nullptr;
			UByte globalAlpha = 200;

			class Renderer {
				using UUIElement = std::unique_ptr<UIElement>;
				UUIElement m_uLabelRoot = nullptr;
				UUIElement m_uBarRoot = nullptr;

			public:
				Renderer() {
					m_uLabelRoot = std::make_unique<UIElement>("Profiler Labels Root", true);
					m_uLabelRoot->InitElement(*pEngine->GetActiveLevel());
					m_uLabelRoot->m_transform.anchor = { 1, 0 };
					m_uLabelRoot->m_transform.size = { textWidth, profilerHeight };
					m_uLabelRoot->m_transform.SetAutoPadNPosition({ -1, -1 });
					m_uLabelRoot->m_layer = LayerID::TOP;
					//m_uLabelRoot->SetPanel(Colour(100, 100, 100, 100));
					m_uBarRoot = std::make_unique<UIElement>("Profiler Bars Root", true);
					m_uBarRoot->InitElement(*pEngine->GetActiveLevel());
					m_uBarRoot->m_transform.anchor = { -1, 0 };
					m_uBarRoot->m_transform.size = Vector2(progressBarSize.x, profilerHeight);
					m_uBarRoot->m_transform.SetAutoPadNPosition({ 1, -1 });
					m_uBarRoot->m_layer = LayerID::TOP;
					//m_uBarRoot->SetPanel(Colour(100, 100, 100, 100));
				}

				void Tick(const Fixed& deltaMS, std::vector<UEntry>& entries) {
					for (auto& uEntry : entries) {
						uEntry->labelElement.Tick(deltaMS);
						uEntry->progressBar.Tick(deltaMS);
					}
				}

				void Render(std::vector<UEntry>& entries) {
					m_uLabelRoot->Render();
					m_uBarRoot->Render();
					int64_t now = SystemClock::GetCurrentMicroseconds();
					for (auto& uEntry : entries) {
						uEntry->labelElement.Render();
						if (uEntry->endUS == 0) uEntry->endUS = now;
						double progress = static_cast<double>(uEntry->ElapsedUS()) / (maxDeltaUS);
						uEntry->progressBar.SetProgress(Fixed(progress));
						uEntry->progressBar.Tick(0);
						uEntry->progressBar.Render();
					}
				}

				void SetupNewEntry(std::vector<UEntry>& entries, Entry& newEntry, Colour colour) {
					Fixed numerator = numEntries - entries.size();
					Fixed nY = numerator / Fixed(numEntries) - Fixed(0.03f);
					nY = (nY * 2) - 1;
					colour.a = globalAlpha;
					newEntry.progressBar.InitElement(*pEngine->GetActiveLevel(), &m_uBarRoot->m_transform);
					newEntry.progressBar.InitProgressBar(progressBarSize, colour);
					newEntry.progressBar.m_layer = LayerID::TOP;
					newEntry.progressBar.m_transform.anchor = { -1, 0 };
					newEntry.progressBar.m_transform.nPosition = { -1, nY };
					newEntry.labelElement.InitElement(*pEngine->GetActiveLevel(), &m_uLabelRoot->m_transform);
					newEntry.labelElement.SetText(UIText(newEntry.id, 20, newEntry.colour));
					newEntry.labelElement.m_layer = LayerID::TOP;
					newEntry.labelElement.m_transform.anchor = { 1, 0 };
					newEntry.labelElement.m_transform.nPosition = { 1, nY };
				}
			};
			using URenderer = std::unique_ptr<Renderer>;
			
			std::vector<UEntry> entries;
			URenderer uRenderer = nullptr;
			bool bEnabled = false;
		}
#pragma endregion

#pragma region Interface
		void Init(Engine& engine) {
			pEngine = &engine;
			maxDeltaUS = 2000;
			textWidth = 300;
			Vector2 worldSize = Graphics::GetGameViewSize();
			profilerHeight = worldSize.y * Fixed(0.7f);
			Fixed screenWidth = worldSize.x;
			progressBarSize = Vector2(screenWidth - textWidth - 10, 10);
			uRenderer = std::make_unique<Renderer>();
		}

		void Toggle(bool bEnable) {
			bEnabled = bEnable;
		}

		void Cleanup() {
			entries.clear();
			uRenderer = nullptr;
		}

		void Tick(const Fixed& deltaMS) {
			if (bEnabled && uRenderer) uRenderer->Tick(deltaMS, entries);
		}

		void Render() {
			if (bEnabled && uRenderer) uRenderer->Render(entries);
		}

		void Reset() {
			entries.clear();
		}

		void Start(const std::string& id, Colour colour) {
			if (!uRenderer) return;
			UEntry uEntry = std::make_unique<Entry>(id, colour, SystemClock::GetCurrentMicroseconds());
			uRenderer->SetupNewEntry(entries, *uEntry, colour);
			entries.emplace_back(std::move(uEntry));
		}

		void Stop(const std::string& id) {
			auto iter = std::find_if(entries.begin(), entries.end(), [id](UEntry& uEntry) { return uEntry->id == id; });
			if (iter != entries.end()) (*iter)->endUS = SystemClock::GetCurrentMicroseconds();
		}
#pragma endregion
#endif
	}
} }
