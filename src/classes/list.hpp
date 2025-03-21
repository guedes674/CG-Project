#ifndef LIST_HPP
#define LIST_HPP

#include <vector>
#include <stdexcept>

template <typename T>
class List {
private:
    std::vector<T> data;
public:
    List() = default;

    // Adiciona um novo valor na lista
    void addValue(const T& value);

    // Obtém o dado de um índice específico
    T getDataByIndex(size_t index) const;

    // Retorna o tamanho da lista
    size_t size() const;

    // Limpa a lista
    void clear();
};

#endif