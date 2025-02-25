#ifndef LIST
#define LIST
#include <stdlib.h>

#define STDSIZE 100

typedef struct list* List;

List newEmptyList();

void* get(List list, long index);

void set(List list, long index, void* element);

int listIsFull(List list);

void add(List list, void* element);

long size(List list);

void remove(List list, long index);

void deleteList(List list);

void deepDeleteList(List list);

#endif