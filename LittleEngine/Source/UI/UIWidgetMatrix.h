#pragma once
#include "CircularVector.hpp"

namespace LittleEngine {
	template<typename T>
	using CircularVector = GameUtils::CircularVector<T>;

	class UIWidgetMatrix {
	private:
		using Vec = CircularVector<std::unique_ptr<class UIWidget>>;
		CircularVector<Vec> m_matrix;
		size_t m_size = 0;
		
	public:
		UIWidgetMatrix();

		void EmplaceWidget(std::unique_ptr<UIWidget> uWidget, bool bNextColumn = false);
		UIWidget* Get();
		
		void Up();
		void Down();
		void Left();
		void Right();
		void Reset(bool bResetRows);
		void Clear();
		void ForEach(std::function<void(std::unique_ptr<UIWidget>&)> Callback);

		size_t TotalCount() const;
		size_t CurrentVecCount();
		size_t NumColumns() const;

	private:
		Vec& GetCurrentVec();
		Vec& GetNewVec();
	};
}
