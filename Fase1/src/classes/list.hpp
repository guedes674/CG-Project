#ifndef LIST_HPP
#define LIST_HPP

#include <iostream>
#include "point.hpp"

typedef struct list* List;

List newL();

void addValueList(List lista, void* data);

void* getData(List node);

List getNext(List node);

void* getDataByIndex(List lista, unsigned long index);

unsigned long size(List lista);

void freeL(List lista);

#endif