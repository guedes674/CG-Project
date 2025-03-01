#include "list.hpp"

List::List() {
    _size = 0;
    capacity = STDSIZE;
    array = (void**)malloc(capacity * sizeof(void*));
}

void* List::get(long index) {
    if (index >= 0 && index < _size) {
        return array[index];
    }
    return nullptr;
}

void List::set(long index, void* element) {
    if (index >= 0 && index < _size) {
        array[index] = element;
    }
}

bool List::listIsFull() {
    return _size == capacity;
}

void List::add(void* element) {
    if (listIsFull()) {
        capacity *= 2;
        array = (void**)realloc(array, capacity * sizeof(void*));
    }
    array[_size++] = element;
}

long List::size() {
    return _size;
}

void List::remove(long index) {
    if (index >= 0 && index < _size) {
        for (long i = index; i < _size - 1; i++) {
            array[i] = array[i + 1];
        }
        _size--;
    }
}

void List::deleteList() {
    free(array);
}

void List::deepDeleteList() {
    for (long i = 0; i < _size; ++i) {
        free(array[i]);
    }
    deleteList();
}

List::~List() {
    deleteList();
}