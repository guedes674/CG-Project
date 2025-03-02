#include "list.hpp"

struct list {
    void* data;
    struct list* next;
};

List newL() {
    return (List)malloc(sizeof(struct list));
}

void addValueList(List lista, void* data) {
    if (lista) {
        List temp = lista;
        while (temp->next != NULL) {
            temp = temp->next;
        }

        List newNode = newL();
        if (newNode) {
            newNode->data = data;
            newNode->next = NULL;
            temp->next = newNode;
        }
    }
}

void* getData(List node) {
    if (node) {
        return node->data;
    }
    return NULL;
}

List getNext(List node) {
    if (node) {
        return node->next;
    }
    return NULL;
}

void* getDataByIndex(List lista, unsigned long index) {
    if (lista) {
        List temp = lista;
        unsigned long i = 0;

        while (i != index && temp != NULL) {
            temp = temp->next;
            i++;
        }

        if (temp != NULL) {
            return temp->data;
        }
    }

    return NULL;
}

unsigned long size(List lista) {
    unsigned long count = 0;
    if (lista) {
        List temp = lista;
        while (temp) {
            count++;
            temp = temp->next;
        }
    }
    return count;
}

void freeL(List lista) {
    while (lista != NULL) {
        List temp = lista;
        lista = lista->next;
        free(temp->data);
        free(temp);
    }
}