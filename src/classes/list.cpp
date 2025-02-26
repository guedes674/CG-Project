#include "list.hpp"

class List {
private:
	void** array;
	long size;
	long capacity;
public:
	List() {
		size = 0;
		capacity = STDSIZE;
		array = (void**)malloc(capacity * sizeof(void*));
	}

	List(array, long size, long capacity) {
		this.array = array;
		this.size = size;
		this.capacity = capacity;
	}

	void get(long index) {
		if (index >= 0 && index < size) {
			return array[index];
		}
		return NULL;
	}

	void set(long index, void* element) {
		if (index >= 0 && index < size) {
			array[index] = element;
		}
	}

	int listIsFull() {
		return size == capacity;
	}

	void add(void* element) {
		if (listIsFull()) {
			size += STDSIZE;
			array = (void**)realloc(array, size * sizeof(void*));
		}
		array[size++] = element;
	}

	long size() {
		return size;
	}

	void remove(long index) {
		if (index >= 0 && index < size) {
			for (long i = index; i < size - 1; i++) {
				array[i] = array[i + 1];
			}
			size--;
		}
	}

	void deleteList() {
		if (array) {
			free(array);
		}
	}

	void deepDeleteList() {
		if (array) {
			for (long i = 0; i < size; i++) {
				free(array[i]);
			}
			deleteList();
		}
	}
};