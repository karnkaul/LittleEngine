#pragma once
#include <functional>
#include "CircularVector.hpp"

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
	UIWidget* Get();

	void Up();
	void Down();
	void Left();
	void Right();
	void Reset(bool bResetRows);
	void Clear();
	void ForEach(const std::function<void(UPtr<UIWidget>&)>& Callback);

	size_t TotalCount() const;
	size_t CurrentVecCount();
	size_t NumColumns() const;

private:
	CVec& GetCurrentVec();
	CVec& GetNewVec();
};
} // namespace LittleEngine
