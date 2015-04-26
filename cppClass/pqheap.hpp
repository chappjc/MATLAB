// pqheap.hpp
//
// A barebones priority queue class to demonstrate how to wrap a C++ class with
// a MEX-file and interface with it via the cppclass MATLAB base-class.
// NOTE: The actual implementation is not included, it will not actually link.
// Contact me if you are actually interested in the PQ implementation.
//
// by Jonathan Chappelow (chappjc)

#ifndef PQHEAP_H_
#define PQHEAP_H_

#include <functional>

//template <typename T, typename C = std::less<T>()>
template <typename T>
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

    int size() const { return N; }
    int capacity() const { return cap; }
    bool empty() const { return N == 0; }

    T const* data() { return heap; }

public:

    static int left_child(int index) { return /*index << 1*/ 2 * index; }
    static int right_child(int index) { return /*(index << 1) + 1*/ 2 * index + 1; }
    static int parent(int index) { return index/2 /*index >> 1*/; }

private:

    int heapUp(int i);
    int heapDown(int i);

    int heapify();

    static void swap(T& a, T& b);

private:

    T*  heap;

    int N;
    int cap;
};

// implementation:
// #include "../src/pqheap.cpp"

#endif
