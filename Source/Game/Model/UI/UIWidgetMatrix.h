#pragma once
#include <memory>
#include "Core/LECoreGame/RingBuffer.h"

namespace LittleEngine
{
template <typename T>
using CircularVector = Core::RingBuffer<T>;

class UIWidgetMatrix
{
private:
	using CVec = CircularVector<std::unique_ptr<class UIWidget>>;
	CircularVector<CVec> m_matrix;
	size_t m_size = 0;

public:
	UIWidgetMatrix();

	void EmplaceWidget(std::unique_ptr<UIWidget> uWidget, bool bNextColumn = false);
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
	void ForEach(std::function<void(std::unique_ptr<UIWidget>&)> callback);
	void ForEach(std::function<void(const std::unique_ptr<UIWidget>&)> callback) const;

	size_t TotalCount() const;
	size_t CurrentVecCount();
	size_t NumColumns() const;

private:
	const CVec& CurrentVec() const;
	CVec& CurrentVec();
	CVec& NewVec();
};
} // namespace LittleEngine
