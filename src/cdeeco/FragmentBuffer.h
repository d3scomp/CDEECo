#ifndef FRAGMENT_BUFFER_H
#define FRAGMENT_BUFFER_H

#include <array>

#include "KnowledgeFragment.h"

template<size_t BUFFER_SIZE>
class FragmentBuffer {
public:
	FragmentBuffer() {
		memset(data.data(), 0, sizeof(data));
	}

	bool isFull() {
		return inc(start) == end;
	}

	bool put(const KnowledgeFragment fragment) {
		if(!isFull()) {
			data[end] = fragment;
			end = inc(end);
			return true;
		} else {
			return false;
		}
	}

	bool isEmpty() {
		return start == end;
	}

	bool get(KnowledgeFragment &fragment) {
		if(!isEmpty()) {
			fragment = data[start];
			start = inc(start);
			return true;
		} else {
			return false;
		}
	}
private:
	std::array<KnowledgeFragment, BUFFER_SIZE> data;
	size_t start = 0;
	size_t end = 0;
	size_t inc(size_t pos) {
		return (pos + 1) % BUFFER_SIZE;
	}
};

#endif // FRAGMENT_BUFFER_H
