#include "list.hpp"

template <typename T>
void List<T>::addValue(const T& value) {
    data.push_back(value);
}

template <typename T>
T List<T>::getDataByIndex(size_t index) const {
    if (index >= data.size()) {
        throw std::out_of_range("Index out of bounds");
    }
    return data[index];
}

template <typename T>
size_t List<T>::size() const {
    return data.size();
}

template <typename T>
void List<T>::clear() {
    data.clear();
}

template class List<int>;
template class List<float>;
template class List<std::string>;