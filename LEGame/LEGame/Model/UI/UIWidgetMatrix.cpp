#include "stdafx.h"
#include "UIWidgetMatrix.h"
#include "UIWidget.h"

namespace LittleEngine
{
UIWidgetMatrix::UIWidgetMatrix()
{
	GetNewVec();
}

void UIWidgetMatrix::EmplaceWidget(UPtr<UIWidget> uWidget, bool bNextColumn)
{
	CVec& current = GetCurrentVec();
	if (current.IsEmpty() || !bNextColumn)
	{
		current.EmplaceBack(std::move(uWidget));
	}
	else
	{
		CVec& newVec = GetNewVec();
		newVec.EmplaceBack(std::move(uWidget));
	}
	++m_size;
}

UIWidget* UIWidgetMatrix::Current()
{
	if (m_matrix.IsEmpty() || m_matrix.GetRef().IsEmpty())
	{
		return nullptr;
	}
	return m_matrix.GetRef().GetRef().get();
}

UIWidget* UIWidgetMatrix::NextSelectableVertical(bool bDownwards)
{
	auto start = GetCurrentVec().GetIter();
	// Go vertical
	bDownwards ? Down() : Up();
	auto iter = GetCurrentVec().GetIter();
	while (iter != start)
	{
		// If current is selectable, return it
		UIWidget* pCurrent = Current();
		if (pCurrent && pCurrent->IsInteractable())
		{
			return pCurrent;
		}
		// Otherwise go vertical and try again
		bDownwards ? Down() : Up();
		iter = GetCurrentVec().GetIter();
	}

	// Back to start (iter == start now)
	UIWidget* pCurrent = Current();
	if (pCurrent && pCurrent->IsInteractable())
	{
		return pCurrent;
	}
	return nullptr;
}

UIWidget* UIWidgetMatrix::NextSelectableHorizontal(bool bRightwards)
{
	auto horzStart = m_matrix.GetIter();
	// Go horizontal
	bRightwards ? Right() : Left();
	auto horzIter = m_matrix.GetIter();
	while (horzIter != horzStart)
	{
		// If current is selectable, return it
		UIWidget* pCurrent = Current();
		if (pCurrent && pCurrent->IsInteractable())
		{
			return pCurrent;
		}
		// Otherwise find next selectable

		pCurrent = NextSelectableVertical(true);
		if (pCurrent)
		{
			return pCurrent;
		}
		// No selectable in column, proceed to next
		bRightwards ? Right() : Left();
		horzIter = m_matrix.GetIter();
	}
	return nullptr;
}

bool UIWidgetMatrix::Select(UIWidget* pToSelect)
{
	Reset(true);
	UIWidget* pHead = Current();
	UIWidget* pRowHead = pHead;
	do
	{
		if (pRowHead == pToSelect)
		{
			return true;
		}
		Down();
		UIWidget* pIter = Current();
		while (pIter != pRowHead)
		{
			if (pIter == pToSelect)
			{
				return true;
			}
			Down();
			pIter = Current();
		}
		Right();
		pRowHead = Current();
	} while (pRowHead != pHead);
	Reset(true);
	return false;
}

void UIWidgetMatrix::Up()
{
	CVec& vec = GetCurrentVec();
	if (!vec.IsEmpty())
	{
		vec.Decrement();
	}
}

void UIWidgetMatrix::Down()
{
	CVec& vec = GetCurrentVec();
	if (!vec.IsEmpty())
	{
		vec.Increment();
	}
}

void UIWidgetMatrix::Left()
{
	if (m_matrix.Size() < 2)
	{
		return;
	}
	m_matrix.Decrement();
	GetCurrentVec().Reset();
}

void UIWidgetMatrix::Right()
{
	if (m_matrix.Size() < 2)
	{
		return;
	}
	m_matrix.Increment();
	GetCurrentVec().Reset();
}

void UIWidgetMatrix::Reset(bool bResetRows)
{
	if (m_matrix.IsEmpty() || GetCurrentVec().IsEmpty())
	{
		return;
	}
	if (bResetRows)
	{
		m_matrix.Reset();
	}
	GetCurrentVec().Reset();
}

void UIWidgetMatrix::Clear()
{
	m_matrix.Clear();
}

void UIWidgetMatrix::ForEach(std::function<void(UPtr<UIWidget>&)> callback)
{
	m_matrix.ForEach([&callback](CVec& vec) { vec.ForEach(callback); });
}

void UIWidgetMatrix::ForEach(std::function<void(const UPtr<UIWidget>&)> callback) const
{
	m_matrix.ForEach([&callback](const CVec& vec) { vec.ForEach(callback); });
}

size_t UIWidgetMatrix::TotalCount() const
{
	return m_size;
}

size_t UIWidgetMatrix::CurrentVecCount()
{
	return GetCurrentVec().Size();
}

size_t UIWidgetMatrix::NumColumns() const
{
	return m_matrix.Size();
}

const UIWidgetMatrix::CVec& UIWidgetMatrix::GetCurrentVec() const
{
	return m_matrix.GetRef();
}

UIWidgetMatrix::CVec& UIWidgetMatrix::GetCurrentVec()
{
	return m_matrix.GetRef();
}

UIWidgetMatrix::CVec& UIWidgetMatrix::GetNewVec()
{
	m_matrix.EmplaceBack(CVec());
	m_matrix.Reset(true);
	return m_matrix.GetRef();
}
} // namespace LittleEngine
