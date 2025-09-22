#pragma once
#include <exception>
#ifdef _DEBUG
#include <vector>
#endif

template <class T>
class typeBoard {
public:
#ifdef _DEBUG
	std::vector<T> data;
#else
	T* data = nullptr;
#endif
	int size = 0;

	typeBoard<T>() {}

	typeBoard<T>(typeBoard<T>* toCopy) {
#ifdef _DEBUG
		data = toCopy->data;
#else
		init(toCopy->size);
		memcpy(data, toCopy->data, memsize());
#endif
	}

	~typeBoard() { 
#ifndef _DEBUG
		if (data) free(data); 
#endif
	}

	void copyFrom(typeBoard<T>* toCopy) {
#ifdef _DEBUG
		data = toCopy->data;
#else
		memcpy(data, toCopy->data, memsize());
#endif
	}

	T& at(int i) { return data[i]; }

	size_t memsize() { return sizeof(T) * size; }

	void init(int size) {
		this->size = size;

#ifdef _DEBUG
		for (int i = 0; i < size; i++)
			data.push_back(0);
#else
		data = (T*)malloc(size * sizeof(T));
		if (data) memset(data, 0, size * sizeof(T));
		else throw std::exception("Could not allocate memory");
#endif
	}
};