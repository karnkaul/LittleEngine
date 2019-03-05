#pragma once
#include "CoreTypes.h"

namespace LittleEngine {
	template<typename T>
	struct CircularList {
		int maxElements = -1;

		CircularList() {
			iter = items.begin();
		}

		void PushBack(const T& value, bool bResetIter = true) {
			items.emplace_back(value);
			if (maxElements > 0 && items.size() > maxElements) {
				items.pop_front();
			}
			if (bResetIter) {
				iter = items.begin();
			}
		}

		void PushFront(const T& value, bool bResetIter = true) {
			items.emplace_front(value);
			if (maxElements > 0 && items.size() > ToIdx(maxElements)) {
				items.pop_back();
			}
			if (bResetIter) {
				iter = items.begin();
			}
		}

		void ResetIter() {
			iter = items.begin();
		}

		bool IsEmpty() const {
			return items.empty();
		}

		// Returns false if overflown (and resets to head)
		bool Increment() {
			if (items.empty()) {
				return false;
			}
			++iter;
			if (iter == items.end()) {
				iter = items.begin();
				return false;
			}
			return true;
		}

		// Returns false if overflown (and resets to tail)
		bool Decrement() {
			if (items.empty()) {
				return false;
			}
			bool bOverflown = false;
			if (iter == items.begin()) {
				iter = items.end();
				bOverflown = true;
			}
			--iter;
			return bOverflown;
		}

		void Clear() {
			items.clear();
			iter = items.begin();
		}

		const T& Get() const {
			return *iter;
		}

	private:
		typename List<T>::const_iterator iter;
		List<T> items;
	};
}
