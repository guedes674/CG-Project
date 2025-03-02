#ifndef LIST_HPP
#define LIST_HPP

#include <iostream>
#include "point.hpp"

template <typename T>
class List {
private:
    T** array;
    long _size;
    long capacity;

    void expandCapacity();

public:
    List(int initial_capacity = 10);
    ~List();

    void add(T* element);
    void set(long index, T* element);
    T* get(long index);
    void remove(long index);
    long size() const;

    void deleteList();
    void deepDeleteList();
};

#endif