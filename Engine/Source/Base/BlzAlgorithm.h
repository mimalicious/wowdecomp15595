#ifndef ENGINE_SOURCE_BASE_BLZALGORITHM_H
#define ENGINE_SOURCE_BASE_BLZALGORITHM_H

#include <cstddef>
#include <cstdint>

namespace blz {
namespace internal {

template <class T>
struct ValueOf;

template <class T>
struct ValueOf<T*> {
    typedef T Type;
};

template <class RandomIt, class Distance, class T, class Compare>
void adjust_heap(RandomIt first, Distance holeIndex, Distance len, const T& value, Compare comp);

template <class RandomIt, class Distance, class T>
void adjust_heap(RandomIt first, Distance holeIndex, Distance len, const T& value);

template <class RandomIt, class Compare>
void insertion_sort(RandomIt first, RandomIt last, Compare comp);

template <class RandomIt>
void insertion_sort(RandomIt first, RandomIt last);

template <class RandomIt, class Size, class Compare>
void introsort_loop(RandomIt first, RandomIt last, Size depthLimit, Compare comp);

template <class RandomIt, class Size>
void introsort_loop(RandomIt first, RandomIt last, Size depthLimit);

template <class RandomIt, class Compare>
RandomIt median(RandomIt a, RandomIt b, RandomIt c, Compare comp);

template <class RandomIt>
RandomIt median(RandomIt a, RandomIt b, RandomIt c);

template <class RandomIt, class Distance, class T, class Compare>
void push_heap(RandomIt first, Distance holeIndex, Distance topIndex, const T& value, Compare comp);

template <class RandomIt, class Distance, class T>
void push_heap(RandomIt first, Distance holeIndex, Distance topIndex, const T& value);

template <class RandomIt, class T, class Compare>
RandomIt unguarded_partition(RandomIt first, RandomIt last, T pivot, Compare comp);

template <class RandomIt, class T>
RandomIt unguarded_partition(RandomIt first, RandomIt last, T pivot);

}

template <class RandomIt, class Compare>
void make_heap(RandomIt first, RandomIt last, Compare comp);

template <class RandomIt>
void make_heap(RandomIt first, RandomIt last);

template <class RandomIt, class Compare>
void partial_sort(RandomIt first, RandomIt middle, RandomIt last, Compare comp);

template <class RandomIt>
void partial_sort(RandomIt first, RandomIt middle, RandomIt last);

template <class RandomIt, class Compare>
void sort(RandomIt first, RandomIt last, Compare comp);

template <class RandomIt>
void sort(RandomIt first, RandomIt last);

template <class RandomIt, class Compare>
void sort_heap(RandomIt first, RandomIt last, Compare comp);

template <class RandomIt>
void sort_heap(RandomIt first, RandomIt last);

}

#include "Engine/Source/Base/BlzAlgorithm.inl"

#endif
