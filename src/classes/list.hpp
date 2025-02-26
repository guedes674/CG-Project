#ifndef LIST_HPP
#define LIST_HPP

#include <cstdlib>

#define STDSIZE 100

class List {
private:
    void** array;
    long _size;
    long capacity;

public:
    List();
    ~List();

    void* get(long index);
    void set(long index, void* element);
    bool listIsFull();
    void add(void* element);
    long size();
    void remove(long index);
    void deleteList();
    void deepDeleteList();
};

#endif
