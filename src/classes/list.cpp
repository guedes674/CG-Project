#include "list.hpp"

struct list{
	void** array;
	long size;
	long capacity;
};

List newEmptyList() {
	List list = (List)malloc(sizeof(struct list));
	if (list) {
		list->array = (void**)calloc(STDSIZE, sizeof(void*));
		if (list->array) {
			list->size = 0;
			list->capacity = STDSIZE;
		}
		else {
			free(list);
			list = NULL;
		}
	}
	return list;
}

void* get(List list, long index) {
	if (list && index >= 0 && index < list->size) {
		return list->array[index];
	}
	return NULL;
}

void set(List list, long index, void* element) {
	if (list && index >= 0 && index < list->size) {
		list->array[index] = element;
	}
}

int listIsFull(List list) {
	if (list) {
		return list->size == list->capacity;
	}
	return 1;
}

void add(List list, void* element) {
	if (list) {
		if (listIsFull(list)) {
			list->size += STDSIZE;
			list->array = (void**)realloc(list->array, list->size * sizeof(void*));
		}
		list->array[list->size++] = element;
	}
}

long size(List list) {
	if (list) {
		return list->size;
	}
	return 0;
}

void remove(List list, long index) {
	if (list && index >= 0 && index < list->size) {
		for (long i = index; i < list->size - 1; i++) {
			list->array[i] = list->array[i + 1];
		}
		list->size--;
	}
}

void deleteList(List list) {
	if (list) {
		free(list->array);
		free(list);
	}
}

void deepDeleteList(List list) {
	if (list) {
		for (long i = 0; i < list->size; i++) {
			free(list->array[i]);
		}
		deleteList(list);
	}
}