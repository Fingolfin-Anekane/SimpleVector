#pragma once

#include <cassert>
#include <initializer_list>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>

#include "array_ptr.h"

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
    :size_(size),
    capacity_(size)
    {
        ArrayPtr<Type> new_arr(size);
        std::fill(&new_arr[0], &new_arr[size], Type());
        arrayPtr_.swap(new_arr);
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
    :size_(size),
    capacity_(size)
    {
        ArrayPtr<Type> new_arr(size);
        std::fill(&new_arr[0], &new_arr[size], value);
        arrayPtr_.swap(new_arr);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
    :size_(init.size()),
    capacity_(init.size())
    {
        ArrayPtr<Type> new_arr(init.size());
        std::copy(init.begin(), init.end(), new_arr.Get());
        arrayPtr_.swap(new_arr);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return !size_;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return arrayPtr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return arrayPtr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) throw std::out_of_range("");
        return arrayPtr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) throw std::out_of_range("");
        return arrayPtr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        if (new_size <= capacity_) {
            std::fill(begin() + size_, begin() + new_size, Type());
            size_ = new_size;
            return;
        }
        // new_size > capacity_
        size_t new_capacity = std::max(new_size, 2 * capacity_);
        Increase(new_capacity);
        std::fill(&arrayPtr_[size_], &arrayPtr_[new_size], Type());
        size_ = new_size;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return &arrayPtr_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return &arrayPtr_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return &arrayPtr_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return &arrayPtr_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return &arrayPtr_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return &arrayPtr_[size_];
    }

    SimpleVector(const SimpleVector& other) {
        ArrayPtr<Type> other_arr(other.size_);
        std::copy(other.begin(), other.end(), other_arr.Get());
        arrayPtr_.swap(other_arr);
        size_ = other.size_;
        capacity_ = other.capacity_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            auto copy(rhs);
            swap(copy);
        }
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (size_ < capacity_) {
            arrayPtr_[size_] = item;
            ++size_;
            return;
        }
        Increase(std::max(capacity_ * 2, static_cast<size_t>(1)));
        PushBack(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        auto dist = std::distance(cbegin(), pos);
        if (size_ >= capacity_) {
            Increase(2*capacity_);
        }
        std::copy_backward(begin() + dist, end(), end() + 1);
        *(begin() + dist) = value;
        ++size_;
        return begin() + dist;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (IsEmpty()) return;
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        size_t dist = std::distance(cbegin(), pos);
        std::copy(&arrayPtr_[dist + 1], &arrayPtr_[size_], &arrayPtr_[dist]);
        return begin() + dist;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        arrayPtr_.swap(other.arrayPtr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    ArrayPtr<Type> arrayPtr_;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void Increase(size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        ArrayPtr<Type> new_arrayPtr(new_capacity);
        std::copy(begin(), end(), new_arrayPtr.Get());
        arrayPtr_.swap(new_arrayPtr);
        capacity_ = new_capacity;
    }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (&lhs == &rhs) || std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    //if (lhs.GetSize() != rhs.GetSize()) return lhs.GetSize() < rhs.GetSize();
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs >= rhs) && (lhs != rhs);
}

template <typename Type>
bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}