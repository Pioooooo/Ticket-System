#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <iterator>

#define ___initCapacity___ 10

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector
    {
        union __ptr
        {
            T *val;
            char *buf;
        } start, finish, end_of_storage;
    
    public:
        /**
             * TODO
             * a type for actions of the elements of a vector, and you should write
             *   a class named const_iterator with same interfaces.
             */
        /**
             * you can see RandomAccessIterator at CppReference for help.
             */
        class const_iterator;
        
        class iterator: public std::iterator<std::random_access_iterator_tag, T>
        {
        private:
            /**
                 * TODO add data members
                 *   just add whatever you want.
                 */
            const vector *f;
            T *p;
        
        public:
            iterator(const vector *const &_f, T *const &_p): f(_f), p(_p)
            {
            }
            
            iterator(const iterator &other) = default;
            
            iterator &operator=(const iterator &other) = default;
            
            /**
                 * return a new iterator which pointer n-next elements
                 *   even if there are not enough elements, just return the answer.
                 * as well as operator-
                 */
            iterator operator+(const int &n) const
            {
                //TODO
                return iterator(f, p + n);
            }
            
            iterator operator-(const int &n) const
            {
                //TODO
                return iterator(f, p - n);
            }
            
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const
            {
                //TODO
                if(f != rhs.f)
                    throw invalid_iterator();
                return (int)(p - rhs.p);
            }
            
            iterator &operator+=(const int &n)
            {
                //TODO
                p += n;
                return *this;
            }
            
            iterator &operator-=(const int &n)
            {
                //TODO
                p -= n;
                return *this;
            }
            
            /**
                 * TODO iter++
                 */
            iterator operator++(int)
            {
                iterator tmp(*this);
                p++;
                return tmp;
            }
            
            /**
                 * TODO ++iter
                 */
            iterator &operator++()
            {
                return *this += 1;
            }
            
            /**
                 * TODO iter--
                 */
            iterator operator--(int)
            {
                iterator tmp(*this);
                p--;
                return tmp;
            }
            
            /**
                 * TODO --iter
                 */
            iterator &operator--()
            {
                return *this -= 1;
            }
            
            /**
                 * TODO *it
                 */
            T &operator*() const
            {
                return *p;
            }
            
            /**
                 * a operator to check whether two iterators are same (pointing to the same memory).
                 */
            bool operator==(const iterator &rhs) const
            {
                return p == rhs.p;
            }
            
            bool operator==(const const_iterator &rhs) const
            {
                return p == rhs.p;
            }
            
            /**
                 * some other operator for iterator.
                 */
            bool operator!=(const iterator &rhs) const
            {
                return !(*this == rhs);
            }
            
            bool operator!=(const const_iterator &rhs) const
            {
                return !(*this == rhs);
            }
            
            bool operator<(const iterator &rhs) const
            {
                return this->p < rhs.p;
            }
            
            bool operator>(const iterator &rhs) const
            {
                return rhs < *this;
            }
            
            bool operator<=(const iterator &rhs) const
            {
                return !(*this > rhs);
            }
            
            bool operator>=(const iterator &rhs) const
            {
                return !(*this < rhs);
            }
        };
        
        /**
             * TODO
             * has same function as iterator, just for a const object.
             */
        class const_iterator: public std::iterator<std::random_access_iterator_tag, const T>
        {
        private:
            /**
                 * TODO add data members
                 *   just add whatever you want.
                 */
            const vector *f;
            const T *p;
        
        public:
            const_iterator(const vector *const &_f, const T *const &_p): f(_f), p(_p)
            {
            }
            
            const_iterator(const const_iterator &other) = default;
            
            const_iterator &operator=(const const_iterator &other) = default;
            
            /**
                 * return a new const_iterator which pointer n-next elements
                 *   even if there are not enough elements, just return the answer.
                 * as well as operator-
                 */
            const_iterator operator+(const int &n) const
            {
                //TODO
                return const_iterator(f, p + n);
            }
            
            const_iterator operator-(const int &n) const
            {
                //TODO
                return const_iterator(f, p - n);
            }
            
            // return th distance between two const_iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const
            {
                //TODO
                if(f != rhs.f)
                    throw invalid_iterator();
                return p - rhs.p;
            }
            
            const_iterator &operator+=(const int &n)
            {
                //TODO
                p += n;
                return *this;
            }
            
            const_iterator &operator-=(const int &n)
            {
                //TODO
                p -= n;
                return *this;
            }
            
            /**
                 * TODO iter++
                 */
            const_iterator operator++(int)
            {
                const_iterator tmp(*this);
                p++;
                return tmp;
            }
            
            /**
                 * TODO ++iter
                 */
            const_iterator &operator++()
            {
                return *this += 1;
            }
            
            /**
                 * TODO iter--
                 */
            const_iterator operator--(int)
            {
                const_iterator tmp(*this);
                p--;
                return tmp;
            }
            
            /**
                 * TODO --iter
                 */
            const_iterator &operator--()
            {
                return *this -= 1;
            }
            
            /**
                 * TODO *it
                 */
            const T &operator*() const
            {
                return *p;
            }
            
            /**
                 * a operator to check whether two iterators are same (pointing to the same memory).
                 */
            bool operator==(const const_iterator &rhs) const
            {
                return p == rhs.p;
            }
            
            bool operator==(const iterator &rhs) const
            {
                return p == rhs.p;
            }
            
            /**
                 * some other operator for const_iterator.
                 */
            bool operator!=(const const_iterator &rhs) const
            {
                return !(*this == rhs);
            }
            
            bool operator!=(const iterator &rhs) const
            {
                return !(*this == rhs);
            }
            
            bool operator<(const const_iterator &rhs) const
            {
                return this->p < rhs.p;
            }
            
            bool operator>(const const_iterator &rhs) const
            {
                return rhs < *this;
            }
            
            bool operator<=(const const_iterator &rhs) const
            {
                return !(*this > rhs);
            }
            
            bool operator>=(const const_iterator &rhs) const
            {
                return !(*this < rhs);
            }
        };
        
        /**
             * TODO Constructs
             * Atleast three: default constructor, copy constructor and a constructor for std::vector
             */
        vector()
        {
            start.buf = finish.buf = new char[sizeof(T) * ___initCapacity___];
            end_of_storage.val = start.val + ___initCapacity___;
        }
        
        vector(const vector &other)
        {
            start.buf = new char[sizeof(T) * other.capacity()];
            finish.val = start.val + other.size();
            end_of_storage.val = start.val + other.capacity();
            for(size_t i = 0; i < other.size(); i++)
                new(start.val + i) T(other[i]);
        }
        
        /**
             * TODO Destructor
             */
        ~vector()
        {
            for(size_t i = 0; i < size(); i++)
                (start.val + i)->~T();
            delete[] start.buf;
        }
        
        /**
             * TODO Assignment operator
             */
        vector &operator=(const vector &other)
        {
            if (this==&other) return *this;
            clear();
            if(capacity() < other.size())
            {
                delete[] start.val;
                start.buf = new char[sizeof(T) * other.capacity()];
            }
            for(size_t i = 0; i < other.size(); i++)
                new(start.val + i) T(other[i]);
            finish.val = start.val + other.size();
            end_of_storage.val = start.val + other.capacity();
            return *this;
        }
        
        /**
             * assigns specified element with bounds checking
             * throw index_out_of_bound if pos is not in [0, size)
             */
        T &at(const size_t &pos)
        {
            if(pos < 0 || pos >= size())
                throw index_out_of_bound();
            return start.val[pos];
        }
        
        const T &at(const size_t &pos) const
        {
            if(pos < 0 || pos >= size())
                throw index_out_of_bound();
            return start.val[pos];
        }
        
        /**
             * assigns specified element with bounds checking
             * throw index_out_of_bound if pos is not in [0, size)
             * !!! Pay attentions
             *   In STL this operator does not check the boundary but I want you to do.
             */
        T &operator[](const size_t &pos)
        {
            if(pos < 0 || pos >= size())
                throw index_out_of_bound();
            return start.val[pos];
        }
        
        const T &operator[](const size_t &pos) const
        {
            if(pos < 0 || pos >= size())
                throw index_out_of_bound();
            return start.val[pos];
        }
        
        /**
             * access the first element.
             * throw container_is_empty if size == 0
             */
        const T &front() const
        {
            if(empty())
                throw container_is_empty();
            return *start.val;
        }
        
        /**
             * access the last element.
             * throw container_is_empty if size == 0
             */
        const T &back() const
        {
            if(empty())
                throw container_is_empty();
            return *(finish.val - 1);
        }
        
        /**
             * returns an iterator to the beginning.
             */
        iterator begin()
        {
            return iterator(this, start.val);
        }
        
        const_iterator cbegin() const
        {
            return const_iterator(this, start.val);
        }
        
        /**
             * returns an iterator to the end.
             */
        iterator end()
        {
            return iterator(this, finish.val);
        }
        
        const_iterator cend() const
        {
            return const_iterator(this, finish.val);
        }
        
        /**
             * checks whether the container is empty
             */
        bool empty() const
        {
            return size() == 0;
        }
        
        /**
             * returns the number of elements
             */
        size_t size() const
        {
            return finish.val - start.val;
        }
        
        /**
             * returns the number of elements that can be held in currently allocated storage.
             */
        size_t capacity() const
        {
            return end_of_storage.val - start.val;
        }
        
        /**
             * clears the contents
             */
        void clear()
        {
            while(finish.val != start.val)
                (--finish.val)->~T();
        }
        
        /**
             * inserts value before pos
             * returns an iterator pointing to the inserted value.
             */
        iterator insert(iterator pos, const T &value)
        {
            size_t _size = pos - begin();
            return insert(_size, value);
        }
        
        /**
             * inserts value at index ind.
             * after inserting, this->at(ind) == value is true
             * returns an iterator pointing to the inserted value.
             * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
             */
        iterator insert(const size_t &ind, const T &value)
        {
            if(ind > size())
                throw index_out_of_bound();
            if(size() == capacity())
                reserve(2 * capacity());
            for(T *p = finish.val; p != start.val + ind; p--)
            {
                new(p) T(*(p - 1));
                (p - 1)->~T();
            }
            new(start.val + ind) T(value);
            finish.val++;
            return iterator(this, start.val + ind);
        }
        
        /**
             * removes the element at pos.
             * return an iterator pointing to the following element.
             * If the iterator pos refers the last element, the end() iterator is returned.
             */
        iterator erase(iterator pos)
        {
            return erase(pos - begin());
        }
        
        /**
             * removes the element with index ind.
             * return an iterator pointing to the following element.
             * throw index_out_of_bound if ind >= size
             */
        iterator erase(const size_t &ind)
        {
            for(T *p = start.val + ind + 1; p != finish.val; p++)
            {
                (p - 1)->~T();
                new(p - 1) T(*p);
            }
            (--finish.val)->~T();
            return iterator(this, start.val + ind);
        }
        
        /**
             * adds an element to the end.
             */
        void push_back(const T &value)
        {
            if(finish.val == end_of_storage.val)
                reserve(2 * capacity());
            new(finish.val++) T(value);
        }
        
        /**
             * remove the last element from the end.
             * throw container_is_empty if size() == 0
             */
        void pop_back()
        {
            (--finish.val)->~T();
        }
        
        void reserve(const size_t &_capacity)
        {
            if(_capacity < capacity())
                return;
            __ptr _start, _finish;
            _start.buf = new char[sizeof(T) * _capacity], _finish.val = _start.val + size();
            for(size_t i = 0; i < size(); i++)
            {
                new(_start.val + i) T(start.val[i]);
                (start.val + i)->~T();
            }
            delete[] start.buf;
            start = _start;
            finish = _finish;
            end_of_storage.val = _start.val + _capacity;
        }
    };
    
} // namespace sjtu

#endif
