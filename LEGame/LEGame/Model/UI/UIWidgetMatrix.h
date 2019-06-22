#pragma once
#include "Core/CircularVector.h"

namespace LittleEngine
{
template <typename T>
using CircularVector = Core::CircularVector<T>;

class UIWidgetMatrix
{
private:
	using CVec = CircularVector<UPtr<class UIWidget>>;
	CircularVector<CVec> m_matrix;
	size_t m_size = 0;

public:
	UIWidgetMatrix();

	void EmplaceWidget(UPtr<UIWidget> uWidget, bool bNextColumn = false);
	UIWidget* Current();
	UIWidget* NextSelectableVertical(bool bDownwards);
	UIWidget* NextSelectableHorizontal(bool bRightwards);
	bool Select(UIWidget* pToSelect);

	void Up();
	void Down();
	void Left();
	void Right();
	void Reset(bool bResetRows);
	void Clear();
	void ForEach(std::function<void(UPtr<UIWidget>&)> callback);
	void ForEach(std::function<void(const UPtr<UIWidget>&)> callback) const;

	size_t TotalCount() const;
	size_t CurrentVecCount();
	size_t NumColumns() const;

private:
	const CVec& GetCurrentVec() const;
	CVec& GetCurrentVec();
	CVec& GetNewVec();
};
} // namespace LittleEngine
