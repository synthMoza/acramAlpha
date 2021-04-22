#pragma once

#include <iostream>
#include <cstddef>
#include <algorithm>
#include <stdexcept>

// Simple implementation of the vector
namespace se_detail {
    // Vector buffer class for vector to inherit from
    template <typename T>
    class vector_buf {
    protected:
        std::size_t size_;
        std::size_t capacity_;
        T* buf_;
    public:
        // Swap the values of these two vector buffers
        void swap(vector_buf& rhs) noexcept {
            std::swap(buf_, rhs.buf_);
            std::swap(size_, rhs.size_);
            std::swap(capacity_, rhs.capacity_);
        }
        // Default constructor for vector buf
        vector_buf() : size_ (0), capacity_ (0), buf_ (nullptr) {}
        // Construct a buffer with same elements of the given size
        vector_buf(std::size_t size, T element) : size_ (size), capacity_ (size), buf_ (nullptr) {
            vector_buf tmp_buf;
            tmp_buf.reserve(size);
            
            for (std::size_t i = 0; i < size; ++i) {
                tmp_buf.buf_[i] = element;
            }

            tmp_buf.size_ = size;

            swap(tmp_buf);
        }
        // Copy constructor
        vector_buf(const vector_buf& rhs) : size_ (0), capacity_ (0), buf_ (nullptr) {
            vector_buf tmp_buf;
            
            // Reserve place
            if (rhs.capacity_ != 0)
                tmp_buf.reserve(rhs.capacity_);

            // Copy elements
            if (rhs.size_ != 0) {
                for (std::size_t i = 0; i < rhs.size_; ++i) {
                    tmp_buf.buf_[i] = rhs.buf_[i];
                }

                tmp_buf.size_ = rhs.size_;
            }

            swap(tmp_buf);
        }
        // Move constuctor
        vector_buf(vector_buf&& rhs) : size_ (rhs.size_), capacity_ (rhs.capacity_), buf_ (rhs.buf_) {
            rhs.buf_ = nullptr;
        }
        // Clears all elemets from the vector, size will equal zero, capacity doesn't change
        void clear() noexcept {
            size_ = 0;
        }
        // Erases the element from the vector buffer
        // If there is no such element, does nothing
        void erase(const T& element) {
            std::size_t i = 0;

            for (i = 0; i < size_; ++i) {
                if (buf_[i] == element)
                    break; 
            }

            if (i != size_) {
                // Element was found
                if (i != size_ - 1) {
                    // Have to change the buffer
                    for (std::size_t j = i; j < size_ - 1; ++j) {
                        // Move elements by one
                        std::swap(buf_[j], buf_[j+1]);
                    }
                }

                size_--;
            }
        }
        // Reserve space for elements
        void reserve(std::size_t new_capacity) {
            // Too little capacity
            if (new_capacity < size_)
                throw std::runtime_error("New capacity is less that the current size of the vector.");
            
            // Reserve place for empty vector buffer (exception safe)
            if (new_capacity != 0 && capacity_ == 0) {
                buf_ = new T[new_capacity];
                capacity_ = new_capacity;
                return ;
            }

            if (capacity_ < new_capacity) {
                // Need to reallocate elements
                vector_buf tmp_buf;
                tmp_buf.reserve(new_capacity);
                
                if (size_ != 0) {
                    // If the vector isn't empty
                    for (std::size_t i = 0; i < size_; ++i) {
                        tmp_buf.buf_[i] = T(std::move(buf_[i]));
                    }

                    tmp_buf.size_ = size_;
                }

                swap(tmp_buf);
            }
        }
        // Return current capacity of this vector buffer
        std::size_t capacity() const {
            return capacity_;
        }
        // Return current size of this vector buffer
        std::size_t size() const {
            return size_;
        }
        virtual ~vector_buf() {
            if (buf_ != nullptr)
                delete[] buf_;
        }
    };
};

namespace se {
    // Vector class that inherits from the vector buffer
    template <typename T>
    class vector : public se_detail::vector_buf<T> {
        // For correct scope
        // Variables
        using se_detail::vector_buf<T>::size_;
        using se_detail::vector_buf<T>::capacity_;
        using se_detail::vector_buf<T>::buf_;
        // Methods
        using se_detail::vector_buf<T>::reserve;

        public:
            // Default constructor
            vector() : se_detail::vector_buf<T>() {} 
            // Create a vector with same elements with the given size
            vector(std::size_t size, T element) : se_detail::vector_buf<T>(size, element) {}
            // Copy constructor
            vector(const vector& rhs) : se_detail::vector_buf<T>(rhs) {}
            // "Steal constructor"
            vector(vector&& rhs) : se_detail::vector_buf<T>(rhs) {}
            // Assignment operator
            vector& operator=(const vector& rhs) {
                vector tmp_vec(rhs);
                se_detail::vector_buf<T>::swap(tmp_vec);
                return *this;
            }
            // Move assignment operator
            vector& operator=(vector&& rhs) {
                vector tmp_vec(rhs);
                se_detail::vector_buf<T>::swap(tmp_vec);
                return *this;
            }
            // Access the element with the given index 
            T& operator[](std::size_t index) const {
                if (index >= size_)
                    throw std::runtime_error("Index is out of range.");

                return buf_[index];
            }
            // Push the element into the vector
            void push_back(T element) {
                std::size_t new_capacity = 0;
                if (size_ == 0) {
                    // Empty vector
                    new_capacity = 1;
                    reserve(new_capacity);   
                } else if (size_ == capacity_) {
                    // Realloc more space
                    new_capacity = 2 * size_;
                    reserve(new_capacity);
                }

                buf_[size_++] = element;
            }
            // Reserve space for elements
            void reserve(std::size_t new_capacity) {
                se_detail::vector_buf<T>::reserve(new_capacity);
            }
            // Erases the element from the vector buffer
            // If there is no such element, does nothing
            void erase(const T& element) {
                se_detail::vector_buf<T>::erase(element);
            }
            // Check whether the vector is empty
            bool empty() const {
                return se_detail::vector_buf<T>::empty();
            }
            // Return current capacity of this vector buffer
            std::size_t capacity() const {
                return se_detail::vector_buf<T>::capacity_;
            }
            // Return current size of this vector buffer
            std::size_t size() const {
                return se_detail::vector_buf<T>::size();
            }
            // Clears all elemets from the vector, size will equal zero, capacity doesn't change
            void clear() noexcept {
                se_detail::vector_buf<T>::clear();
            }
    };
};