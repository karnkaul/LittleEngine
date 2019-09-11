#include "UIWidgetMatrix.h"
#include "UIWidget.h"

namespace LittleEngine
{
UIWidgetMatrix::UIWidgetMatrix()
{
	NewVec();
}

void UIWidgetMatrix::EmplaceWidget(UPtr<UIWidget> uWidget, bool bNextColumn)
{
	CVec& current = CurrentVec();
	if (current.IsEmpty() || !bNextColumn)
	{
		current.EmplaceBack(std::move(uWidget));
	}
	else
	{
		CVec& newVec = NewVec();
		newVec.EmplaceBack(std::move(uWidget));
	}
	++m_size;
}

UIWidget* UIWidgetMatrix::Current()
{
	if (m_matrix.IsEmpty() || m_matrix.Ref().IsEmpty())
	{
		return nullptr;
	}
	return m_matrix.Ref().Ref().get();
}

UIWidget* UIWidgetMatrix::NextSelectableVertical(bool bDownwards)
{
	auto start = CurrentVec().Iter();
	// Go vertical
	bDownwards ? Down() : Up();
	auto iter = CurrentVec().Iter();
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
		iter = CurrentVec().Iter();
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
	auto horzStart = m_matrix.Iter();
	// Go horizontal
	bRightwards ? Right() : Left();
	auto horzIter = m_matrix.Iter();
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
		horzIter = m_matrix.Iter();
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
	CVec& vec = CurrentVec();
	if (!vec.IsEmpty())
	{
		vec.Decrement();
	}
}

void UIWidgetMatrix::Down()
{
	CVec& vec = CurrentVec();
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
	CurrentVec().Reset();
}

void UIWidgetMatrix::Right()
{
	if (m_matrix.Size() < 2)
	{
		return;
	}
	m_matrix.Increment();
	CurrentVec().Reset();
}

void UIWidgetMatrix::Reset(bool bResetRows)
{
	if (m_matrix.IsEmpty() || CurrentVec().IsEmpty())
	{
		return;
	}
	if (bResetRows)
	{
		m_matrix.Reset();
	}
	CurrentVec().Reset();
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
	return CurrentVec().Size();
}

size_t UIWidgetMatrix::NumColumns() const
{
	return m_matrix.Size();
}

const UIWidgetMatrix::CVec& UIWidgetMatrix::CurrentVec() const
{
	return m_matrix.Ref();
}

UIWidgetMatrix::CVec& UIWidgetMatrix::CurrentVec()
{
	return m_matrix.Ref();
}

UIWidgetMatrix::CVec& UIWidgetMatrix::NewVec()
{
	m_matrix.EmplaceBack(CVec());
	m_matrix.Reset(true);
	return m_matrix.Ref();
}
} // namespace LittleEngine
