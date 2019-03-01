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
	Vec& current = GetCurrentVec();
	if (current.IsEmpty() || !bNextColumn)
	{
		current.EmplaceBack(std::move(uWidget));
	}
	else
	{
		Vec& newVec = GetNewVec();
		newVec.EmplaceBack(std::move(uWidget));
	}
	++m_size;
}

UIWidget* UIWidgetMatrix::Get()
{
	if (m_matrix.IsEmpty() || m_matrix.GetRef().IsEmpty())
		return nullptr;
	return m_matrix.GetRef().GetRef().get();
}

void UIWidgetMatrix::Up()
{
	Vec& vec = GetCurrentVec();
	if (!vec.IsEmpty())
		vec.Decrement();
}

void UIWidgetMatrix::Down()
{
	Vec& vec = GetCurrentVec();
	if (!vec.IsEmpty())
		vec.Increment();
}

void UIWidgetMatrix::Left()
{
	if (m_matrix.Size() < 2)
		return;
	m_matrix.Decrement();
	GetCurrentVec().Reset();
}

void UIWidgetMatrix::Right()
{
	if (m_matrix.Size() < 2)
		return;
	m_matrix.Increment();
	GetCurrentVec().Reset();
}

void UIWidgetMatrix::Reset(bool bResetRows)
{
	if (m_matrix.IsEmpty() || GetCurrentVec().IsEmpty())
		return;
	if (bResetRows)
		m_matrix.Reset();
	GetCurrentVec().Reset();
}

void UIWidgetMatrix::Clear()
{
	m_matrix.Clear();
}

void UIWidgetMatrix::ForEach(const std::function<void(UPtr<UIWidget>&)>& Callback)
{
	m_matrix.ForEach([&Callback](Vec& vec) { vec.ForEach(Callback); });
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

UIWidgetMatrix::Vec& UIWidgetMatrix::GetCurrentVec()
{
	return m_matrix.GetRef();
}

UIWidgetMatrix::Vec& UIWidgetMatrix::GetNewVec()
{
	m_matrix.EmplaceBack(Vec());
	m_matrix.Reset(true);
	return m_matrix.GetRef();
}
} // namespace LittleEngine
