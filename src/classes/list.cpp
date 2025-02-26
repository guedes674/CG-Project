#include "list.hpp"
#include <cstdlib>

class List {
private:
    void** array;
    long _size;
    long capacity;

public:
    List() {
        _size = 0;
        capacity = STDSIZE;
        array = (void**)malloc(capacity * sizeof(void*));
    }

    void* get(long index) {
        if (index >= 0 && index < _size) {
            return array[index];
        }
        return NULL;
    }

    void set(long index, void* element) {
        if (index >= 0 && index < _size) {
            array[index] = element;
        }
    }

    bool listIsFull() {
        return _size == capacity;
    }

    void add(void* element) {
        if (listIsFull()) {
            capacity += STDSIZE;
            array = (void**)realloc(array, capacity * sizeof(void*));
        }
        array[_size++] = element;
    }

    long size() {
        return _size;
    }

    void remove(long index) {
        if (index >= 0 && index < _size) {
            for (long i = index; i < _size - 1; i++) {
                array[i] = array[i + 1];
            }
            _size--;
        }
    }

    void deleteList() {
        if (array) {
            free(array);
            array = NULL;
        }
    }

    void deepDeleteList() {
        if (array) {
            for (long i = 0; i < _size; i++) {
                free(array[i]);
            }
            deleteList();
        }
    }

    ~List() {
        deleteList();
    }
};
