// pqheap.hpp - The PQheap class implementation (included by header, do not
//              compilie directly).
// by Jonathan Chappelow (chappjc)

#include <cassert>

template <typename T, typename C>
PQheap<T,C>::PQheap()
: heap(NULL), N(0), cap(0)
{
}

template <typename T, typename C>
PQheap<T,C>::PQheap(unsigned int maxSize)
: heap(NULL)
{
    init(cap);
}

template <typename T, typename C>
int PQheap<T,C>::init(unsigned int maxSize)
{
    N = 0;
    cap = maxSize;
	
	if (maxSize == 0)
        return 0;

    if (heap)
        delete[] heap;

    heap = new T[cap + 1];

    if (!heap) {
        cap = 0;
        return 1;
    }

    return 0;
}

template <typename T, typename C>
PQheap<T,C>::~PQheap()
{
    if (heap) delete[] heap;
    N = 0;
    cap = 0;
}

template <typename T, typename C>
int PQheap<T,C>::insert(const T& t)
{
    if (cap == 0)
        return -2;

    if (N == cap) {
        if (cmp(t, heap[1])) {
            // replace top and sift down
            heap[1] = t;
            return heapDownQuick();
        }
        else {
            return -1;
        }
    }

    // append bottom and bubble up
    heap[++N] = t;

    return N==1 ? 1 : heapUp();
}

template <typename T, typename C>
int PQheap<T,C>::heapUp()
{
    return heapUp(N);
    /*assert(N > 1);

    unsigned int i = N;
    unsigned int p = parent(i);

    while (i > 1 && heap[p] > heap[i]) {
        swap(heap[p], heap[i]);
        i = p;
        p = parent(i);
    }

    return i;*/
}

template <typename T, typename C>
int PQheap<T,C>::heapUp(int i)
{
    assert(N > 1);

    T bubble = heap[i];

    //unsigned int i = N;
    int p = parent(i);

    while (i > 1 && cmp(heap[p], bubble) /*heap[i]*/) {
        //swap(heap[p], heap[i]);
        heap[i] = heap[p];
        i = p;
        p = parent(i);
    }

    heap[i] = bubble;

    return i;
}

template <typename T, typename C>
int PQheap<T,C>::extractTop(T& t)
{
    if (N < 1) return 1;

    t = heap[1];

    if (N > 1)
        heap[1] = heap[N];

#ifdef _DEBUG
    heap[N--] = T();
#else
    --N;
#endif

    return N <= 1 ? 0 : heapDownQuick();
}

template <typename T, typename C>
T PQheap<T,C>::peakTop() const
{
    return N > 0 ? heap[1] : T();
}

template <typename T, typename C>
int PQheap<T,C>::peakTop(const T* t) const
{
    if (N < 1) return 1;

    t = &heap[1];

    return 0;
}

template <typename T, typename C>
void PQheap<T,C>::discardTop()
{
    if (N < 1) return;
    if (N > 1) {
        heap[1] = heap[N--];
        heapDownQuick();
    }
    else
        --N;
}

template <typename T, typename C>
T PQheap<T,C>::extractTop()
{
    T top;
    extractTop(top);
    return top;
}

//template <typename T>
//int PQheap<T>::heapDown()
//{
//    //return heapDown(1);
//    assert(N > 1);
//
//    int i = 1;
//    int left = 2;  // left_child(1)
//    int right = 3; // right_child(1)
//
//    if (right <= N && heap[left] > heap[right])
//        left = right;
//
//    while (left <= N && heap[i] > heap[left]) {
//        swap(heap[left], heap[i]);
//
//        i = left;
//        left = left_child(i);
//        right = left + 1;
//
//        if (right <= N && heap[left] > heap[right])
//            left = right;
//    }
//
//    return i;
//}

template <typename T, typename C>
int PQheap<T,C>::heapDown()
{
    //return heapDown(1);
    assert(N > 1);

    int i = 1;
    int next = 2;  // left_child(1)

    T rock = heap[i];

    if (next < N && cmp(heap[next] , heap[next+1]))
        ++next; // go right

    while (next <= N && cmp(rock , heap[next])) {
        heap[i] = heap[next];

        i = next;
        next = left_child(i);

        if (next < N && cmp(heap[next] , heap[next + 1]))
            ++next;
    }

    heap[i] = rock;

    return i;
}

template <typename T, typename C>
int PQheap<T,C>::heapDownQuick()
{
    //return heapDown(1);
    assert(N > 1);

    int i = 1;
    int next = 2;  // left_child(1)

    T rock = heap[i];

    while (i <= N/2) {
        next = left_child(i);

        if (next < N && cmp(heap[next] , heap[next + 1]))
            ++next;
        if (!cmp(rock , heap[next]))
            break;

        heap[i] = heap[next];
        i = next;
    }

    heap[i] = rock;

    return i;
}


template <typename T, typename C>
int PQheap<T,C>::heapDown(int i)
{
    assert(N > 1);

    int left = left_child(i);
    int right = right_child(i);

    if (right <= N && cmp(heap[left] , heap[right]))
        left = right;

    while (left <= N && cmp(heap[i] , heap[left])) {
        swap(heap[left], heap[i]);

        i = left;
        left = left_child(i);
        right = left + 1;

        if (right <= N && cmp(heap[left] , heap[right]))
            left = right;
    }

    return i;
}

template <typename T, typename C>
int PQheap<T,C>::heapify()
{
    return heapDown();
}

#if _MSC_VER < 1800
template <typename T, typename C>
void PQheap<T,C>::swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}
#else
// #if __cplusplus >= 201103L
#define XSTR(x) STR(x)
#define STR(x) #x
#pragma message("C++: " XSTR(__cplusplus) ", MSVC: " XSTR(_MSC_VER))
template <typename T, typename C>
void PQheap<T,C>::swap(T& a, T& b) {
    T tmp(std::move(a));
    a = std::move(b);
    b = std::move(tmp);
}
#endif