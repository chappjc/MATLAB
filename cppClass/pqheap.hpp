// pqheap.hpp
//
// Template parameters:
//    1. typename T - the data type of each element.  Can be any type for which
//       the comparator specified in the next parameter is defined.
//    2. typename C - A functor: std::less<T> or std::greater<T>.  To create a
//       min-oriented queue (top is smallest in N largest elements), use 
//       std::greater<T>. To create a max-oriented queue (top is largest in N
//       smallest elements), use std::less<T>.
//
// by Jonathan Chappelow (chappjc)

#ifndef PQHEAP_H_
#define PQHEAP_H_

#include <functional>

//template <typename T>
template <typename T, typename C = std::less<T>>
class PQheap
{
public:
    
    PQheap();
    ~PQheap();

    int init(unsigned int maxSize);
    PQheap(unsigned int maxSize);

    typedef typename T element_type;

public:

    int insert(const T& t);

    int extractTop(T& t);
    T extractTop();

    int peakTop(const T* t) const;
    T peakTop() const;
    void discardTop();
    int size() const { return N; }
    int capacity() const { return cap; }
    bool empty() const { return N == 0; }

    T const* data() { return heap; }

public:

    static int left_child(int index) { return /*index << 1*/ 2 * index; }
    static int right_child(int index) { return /*(index << 1) + 1*/ 2 * index + 1; }
    static int parent(int index) { return index/2 /*index >> 1*/; }

private:

    int heapUp();
    int heapDown();
    int heapDownQuick();

    // duplicate code for speed, no default arguments
    int heapUp(int i);
    int heapDown(int i); // no default, use overload

    int heapify();

    static void swap(T& a, T& b);

private:

    T*  heap;

    int N;
    int cap;
    C cmp;
};

// actual implementation separate to keep this file short
#include "pqheap.cpp"

#endif
