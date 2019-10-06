#include "Core/LECoreGame/ArchiveReader.h"
#include "Core/LECoreGame/GData.h"
#include "Core/LECoreGame/LECoreUtils/Logger.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Repository/LERepository.h"
#include "Model/GameManager.h"
#include "Framework/UI/UI.h"
#include "Framework/Utility/Debug/Console/DebugCommands.h"
#include "Framework/Utility/Debug/Console/LogLine.h"
#include "UIContainer.h"

namespace LittleEngine
{
using GData = Core::GData;

#if ENABLED(CONSOLE)
namespace Debug
{
extern Colour g_logTextColour;
extern Colour g_logWarningColour;

namespace Commands
{
CMD_UIContainer::CMD_UIContainer() : Command("ui_container")
{
	m_bTakesCustomParam = true;
}

void CMD_UIContainer::FillExecuteResult(std::string params)
{
	if (params.empty())
	{
		m_executeResult.emplace_back("Syntax: " + m_name + " <assetID> / clear / reset (if UIContainer no longer exists)", g_logTextColour);
	}
	else
	{
		if (params == "clear")
		{
			if (m_pContainer)
			{
				m_pContainer->Destruct();
				m_pContainer = nullptr;
			}
		}
		else if (params == "reset")
		{
			m_pContainer = nullptr;
		}
		else
		{
			auto pGData = g_pRepository->Load<TextAsset>(params, true);
			if (!pGData)
			{
				m_executeResult.emplace_back("Asset ID not found: " + params, g_logWarningColour);
			}
			else
			{
				Assert(g_pGameManager, "Game Manager is null!");
				m_pContainer = g_pGameManager->UI()->PushContext<UIContainer>("ConsoleUIContainer");
				if (m_pContainer)
				{
					m_pContainer->Deserialise(pGData->Text());
				}
			}
		}
	}
};
} // namespace Commands
} // namespace Debug
#endif

UIContainer::UIContainer() = default;
UIContainer::~UIContainer() = default;

void UIContainer::Deserialise(std::string serialised)
{
	GData root(std::move(serialised));
	m_bAutoDestroyOnCancel = root.GetBool("isDestroyOnCancel", true);

	// Setup root
	if (root.Contains("size"))
	{
		m_pRoot->SetRectSize(root.GetVector2("size"));
	}
	if (root.GetBool("isPanel"))
	{
		m_pRoot->SetPanel(UIGameStyle::ParseColour(root.GetString("colour")));
	}
	// Setup children
	SetupChildren(m_pRoot, root.GetVectorGData("children"));
	m_pRoot->SetStatic(true);

	SetPersistent(root.GetBool("persistent", false));
}

void UIContainer::SetupChildren(UIElement* pParent, std::vector<GData> uiObjects)
{
	for (auto& data : uiObjects)
	{
		UIObject* pObj = nullptr;
		UIElement* pNextParent = nullptr;
		s32 layerDelta = data.GetS32("layerDelta", 0);
		std::string id = data.GetString("id");
		std::string name;
		name.reserve(m_name.size() + id.size() + 1);
		name += m_name;
		name += "_";
		name += id;
#if !defined(SHIPPING)
		if (!id.empty())
		{
			auto duplicate = std::find_if(m_objs.begin(), m_objs.end(), [&id](const Obj& obj) { return obj.id == id; });
			Assert(duplicate == m_objs.end(), "Duplicate ID encountered while deserialising UIContainer!");
			if (duplicate != m_objs.end())
			{
				LOG_E("[UIContainer] Duplicate ID encountered! [%s]", id.c_str());
			}
		}
#endif
		std::string widgetType = data.GetString("widgetType");
		Vector2 position = data.GetVector2("position", Vector2::Zero);
		Vector2 size = data.GetVector2("size", {-1, -1});
		GData uiTextData = data.GetString("uiText");
		std::string text = uiTextData.GetString("text");
		u16 textSize = static_cast<u16>(uiTextData.GetS32("size", UIText::s_DEFAULT_PIXEL_SIZE));
		Colour textColour = UIGameStyle::ParseColour(uiTextData.GetString("colour"));
		bool bAutoPad = data.GetBool("isAutoPad", false);
		if (text.empty())
		{
			text = data.GetString("text");
		}

		Strings::ToLower(widgetType);
		if (widgetType.empty())
		{
			auto pElement = AddElement<UIElement>(name, &pParent->m_transform, layerDelta);
			pElement->m_transform.nPosition = position;
			if (size.x > Fixed::Zero && size.y > Fixed::Zero)
			{
				pElement->SetRectSize(size, bAutoPad);
			}
			if (!text.empty())
			{
				pElement->SetText(UIText(text, textSize, textColour));
			}
			if (data.GetBool("isPanel"))
			{
				pElement->SetPanel(UIGameStyle::ParseColour(data.GetString("colour")), data.GetS32("border", 0),
								   UIGameStyle::ParseColour(data.GetString("outline")));
			}

			pNextParent = pElement;
			pObj = pElement;
		}
		else
		{
			bool bNewColumn = data.GetBool("isNewColumn", false);
			UIWidget* pWidget = nullptr;

			UIWidgetStyle style = UIGameStyle::GetStyle(data.GetString("styleID", ""));
			UIWidgetStyle* pStyle = &style;

			if (widgetType == "button")
			{
				if (size.x > Fixed::Zero && size.y > Fixed::Zero)
				{
					pStyle->widgetSize = size;
				}
				auto pButton = AddWidget<UIButton>(name, pStyle, bNewColumn);
				if (!text.empty())
				{
					pButton->SetText(UIText(text, textSize, textColour));
				}
				pWidget = pButton;
			}

			else if (widgetType == "toggle")
			{
				if (data.GetString("styleID").empty())
				{
					style = UIGameStyle::GetStyle("UIToggle");
				}
				auto pToggle = AddWidget<UIToggle>(name, pStyle, bNewColumn);
				pToggle->SetOn(data.GetBool("isOn", true));
				if (size.x > Fixed::Zero && size.y > Fixed::Zero)
				{
					pToggle->SetBoxSize(size);
				}
				if (!text.empty())
				{
					pToggle->SetText(UIText(text, textSize, textColour));
				}
				std::string dump = data.GetString("onColour");
				if (!dump.empty())
				{
					pToggle->SetOnColour(UIGameStyle::ParseColour(dump));
				}
				dump = data.GetString("offColour");
				if (!dump.empty())
				{
					pToggle->SetOffColour(UIGameStyle::ParseColour(dump));
				}
				dump = data.GetString("background");
				if (!dump.empty())
				{
					pToggle->SetBackground(UIGameStyle::ParseColour(dump));
				}
				pWidget = pToggle;
			}

			else if (widgetType == "selection")
			{
				auto pSelection = AddWidget<UISelection>(name, pStyle, bNewColumn);
				pSelection->SetText(UIText(text, textSize, textColour));
				pSelection->SetOptions(data.GetVector("options"));
				pWidget = pSelection;
			}

			else if (widgetType == "textInput")
			{
				auto pTextInput = AddWidget<UITextInput>(name, pStyle, bNewColumn);
				pWidget = pTextInput;
			}

			else
			{
				LOG_E("[UIContainer] Unsupported UIWidget type! [%s]", widgetType.c_str());
			}

			// Generic Widget setup
			if (pWidget)
			{
				pWidget->SetInteractable(data.GetBool("isInteractable", true));
				pObj = pWidget;

				pNextParent = pWidget->Root();
				pNextParent->m_transform.nPosition = position;
				pNextParent->m_transform.posDelta = data.GetVector2("padding", Vector2::Zero);
				if (size.x > Fixed::Zero && size.y > Fixed::Zero)
				{
					pNextParent->SetRectSize(size, bAutoPad);
				}
			}
		}

		pNextParent->SetParent(pParent->m_transform);

		if (pObj)
		{
			pObj->Tick();
			m_objs.push_back({id, pObj});
		}

		SetupChildren(pNextParent, data.GetVectorGData("children"));
	}
}

UIObject* UIContainer::GetObj(const std::string& id)
{
	auto search = std::find_if(m_objs.begin(), m_objs.end(), [&id](const Obj& obj) { return obj.id == id; });
	if (search != m_objs.end())
	{
		return search->pObj;
	}
	return nullptr;
}
} // namespace LittleEngine
