// OFFSET: [0x00736680<DDMaterial**, int32_t, DDMaterial*, DDMaterialSort>, 0x008151C0<CModelBatch*, int32_t, CModelBatch, SModelBatchLessThan>, 0x00839200<uint32_t*, int32_t, uint32_t, M2SortCompare>, 0x009F8860<C2iVector*, int32_t, C2iVector, bool (*)(const C2iVector&, const C2iVector&)>]
template <class RandomIt, class Distance, class T, class Compare>
void blz::internal::push_heap(RandomIt first, Distance holeIndex, Distance topIndex, const T& value, Compare comp) {
    Distance parent = (holeIndex - 1) / 2;

    while (holeIndex > topIndex && comp(first[parent], value)) {
        first[holeIndex] = first[parent];
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }

    first[holeIndex] = value;
}

// OFFSET: [0x00736ED0<DDMaterial**, int32_t, DDMaterial*, DDMaterialSort>, 0x008162F0<CModelBatch*, int32_t, CModelBatch, SModelBatchLessThan>, 0x00839300<uint32_t*, int32_t, uint32_t, M2SortCompare>, 0x009F8E00<C2iVector*, int32_t, C2iVector, bool (*)(const C2iVector&, const C2iVector&)>]
template <class RandomIt, class Distance, class T, class Compare>
void blz::internal::adjust_heap(RandomIt first, Distance holeIndex, Distance len, const T& value, Compare comp) {
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;

    while (secondChild < len) {
        if (comp(first[secondChild], first[secondChild - 1])) {
            secondChild--;
        }

        first[holeIndex] = first[secondChild];
        holeIndex = secondChild;
        secondChild = 2 * secondChild + 2;
    }

    if (secondChild == len) {
        first[holeIndex] = first[secondChild - 1];
        holeIndex = secondChild - 1;
    }

    blz::internal::push_heap(first, holeIndex, topIndex, value, comp);
}

// OFFSET: [0x00736500<DDMaterial**, DDMaterialSort>, 0x00815160/0x14052D350<CModelBatch*, SModelBatchLessThan>, 0x00839110<uint32_t*, M2SortCompare>, 0x009F87E0<C2iVector*, bool (*)(const C2iVector&, const C2iVector&)>]
template <class RandomIt, class Compare>
void blz::internal::insertion_sort(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) {
        return;
    }

    for (RandomIt i = first + 1; i != last; i++) {
        RandomIt j = i;
        typename blz::internal::ValueOf<RandomIt>::Type value = *i;

        for (; j != first; j--) {
            if (!comp(value, *(j - 1))) {
                break;
            }

            *j = *(j - 1);
        }

        *j = value;
    }
}

// OFFSET: [0x00736570<DDMaterial**, DDMaterialSort>, 0x00839170<uint32_t*, M2SortCompare>]
template <class RandomIt, class Compare>
RandomIt blz::internal::median(RandomIt a, RandomIt b, RandomIt c, Compare comp) {
    if (comp(*a, *b)) {
        if (comp(*b, *c)) {
            return b;
        }

        if (comp(*a, *c)) {
            return c;
        }

        return a;
    }

    if (comp(*a, *c)) {
        return a;
    }

    if (comp(*b, *c)) {
        return c;
    }

    return b;
}

// OFFSET: [0x00736E50<DDMaterial**, DDMaterial*, DDMaterialSort>, 0x00816290<CModelBatch*, CModelBatch, SModelBatchLessThan>, 0x00839280<uint32_t*, uint32_t, M2SortCompare>, 0x009F8D80<C2iVector*, C2iVector, bool (*)(const C2iVector&, const C2iVector&)>]
template <class RandomIt, class T, class Compare>
RandomIt blz::internal::unguarded_partition(RandomIt first, RandomIt last, T pivot, Compare comp) {
    while (true) {
        while (comp(*first, pivot)) {
            first++;
        }

        last--;

        while (comp(pivot, *last)) {
            last--;
        }

        if (!(first < last)) {
            return first;
        }

        typename blz::internal::ValueOf<RandomIt>::Type tmp = *first;
        *first = *last;
        *last = tmp;
        first++;
    }
}

// OFFSET: [0x00737F20<DDMaterial**, DDMaterialSort>, 0x0081C2A0<CModelBatch*, SModelBatchLessThan>, 0x00839380<uint32_t*, M2SortCompare>, 0x009F9000<C2iVector*, bool (*)(const C2iVector&, const C2iVector&)>]
template <class RandomIt, class Compare>
void blz::make_heap(RandomIt first, RandomIt last, Compare comp) {
    int32_t len = last - first;

    if (len < 2) {
        return;
    }

    int32_t parent = (len - 2) / 2;

    while (true) {
        typename blz::internal::ValueOf<RandomIt>::Type value = first[parent];
        blz::internal::adjust_heap(first, parent, len, value, comp);

        if (parent == 0) {
            return;
        }

        parent--;
    }
}

// OFFSET: [0x00737F90<DDMaterial**, DDMaterialSort>, 0x0081C340<CModelBatch*, SModelBatchLessThan>, 0x008393F0<uint32_t*, M2SortCompare>, 0x009F9090<C2iVector*, bool (*)(const C2iVector&, const C2iVector&)>]
template <class RandomIt, class Compare>
void blz::sort_heap(RandomIt first, RandomIt last, Compare comp) {
    while (last - first > 1) {
        last--;
        typename blz::internal::ValueOf<RandomIt>::Type value = *last;
        *last = *first;
        blz::internal::adjust_heap(first, 0, static_cast<int32_t>(last - first), value, comp);
    }
}

// OFFSET: [0x007393E0<DDMaterial**, DDMaterialSort>, 0x0081D7E0<CModelBatch*, SModelBatchLessThan>, 0x00839450<uint32_t*, M2SortCompare>, 0x009F9790<C2iVector*, bool (*)(const C2iVector&, const C2iVector&)>]
template <class RandomIt, class Compare>
void blz::partial_sort(RandomIt first, RandomIt middle, RandomIt last, Compare comp) {
    blz::make_heap(first, middle, comp);

    for (RandomIt i = middle; i < last; i++) {
        if (comp(*i, *first)) {
            typename blz::internal::ValueOf<RandomIt>::Type value = *i;
            *i = *first;
            blz::internal::adjust_heap(first, 0, static_cast<int32_t>(middle - first), value, comp);
        }
    }

    blz::sort_heap(first, middle, comp);
}

// OFFSET: [0x00739520<DDMaterial**, int32_t, DDMaterialSort>, 0x0081F270<CModelBatch*, int32_t, SModelBatchLessThan>, 0x008394D0<uint32_t*, int32_t, M2SortCompare>, 0x009F9810<C2iVector*, int32_t, bool (*)(const C2iVector&, const C2iVector&)>]
template <class RandomIt, class Size, class Compare>
void blz::internal::introsort_loop(RandomIt first, RandomIt last, Size depthLimit, Compare comp) {
    while (last - first > 32) {
        if (depthLimit == 0) {
            blz::partial_sort(first, last, last, comp);
            return;
        }

        depthLimit--;
        RandomIt cut = blz::internal::unguarded_partition(first, last, *blz::internal::median(first, first + (last - first) / 2, last - 1, comp), comp);
        blz::internal::introsort_loop(cut, last, depthLimit, comp);
        last = cut;
    }
}

// OFFSET: [0x00820050<CModelBatch*, SModelBatchLessThan>, 0x00839580/0x140563920<uint32_t*, M2SortCompare>]
template <class RandomIt, class Compare>
void blz::sort(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) {
        return;
    }

    int32_t depth = 0;

    for (int32_t n = last - first; n != 1; n >>= 1) {
        depth++;
    }

    blz::internal::introsort_loop(first, last, depth * 2, comp);
    blz::internal::insertion_sort(first, last, comp);
}

// INLINED
template <class RandomIt, class Distance, class T>
void blz::internal::push_heap(RandomIt first, Distance holeIndex, Distance topIndex, const T& value) {
    Distance parent = (holeIndex - 1) / 2;

    while (holeIndex > topIndex && first[parent] < value) {
        first[holeIndex] = first[parent];
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }

    first[holeIndex] = value;
}

// OFFSET: [0x0077D590<LiquidChunkAndDistance*, int32_t, LiquidChunkAndDistance>]
template <class RandomIt, class Distance, class T>
void blz::internal::adjust_heap(RandomIt first, Distance holeIndex, Distance len, const T& value) {
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;

    while (secondChild < len) {
        if (first[secondChild] < first[secondChild - 1]) {
            secondChild--;
        }

        first[holeIndex] = first[secondChild];
        holeIndex = secondChild;
        secondChild = 2 * secondChild + 2;
    }

    if (secondChild == len) {
        first[holeIndex] = first[secondChild - 1];
        holeIndex = secondChild - 1;
    }

    blz::internal::push_heap(first, holeIndex, topIndex, value);
}

// OFFSET: [0x0077CD70/0x1404697F0<LiquidChunkAndDistance*>]
template <class RandomIt>
void blz::internal::insertion_sort(RandomIt first, RandomIt last) {
    if (first == last) {
        return;
    }

    for (RandomIt i = first + 1; i != last; i++) {
        RandomIt j = i;
        typename blz::internal::ValueOf<RandomIt>::Type value = *i;

        for (; j != first; j--) {
            if (!(value < *(j - 1))) {
                break;
            }

            *j = *(j - 1);
        }

        *j = value;
    }
}

// INLINED
template <class RandomIt>
RandomIt blz::internal::median(RandomIt a, RandomIt b, RandomIt c) {
    if (*a < *b) {
        if (*b < *c) {
            return b;
        }

        if (*a < *c) {
            return c;
        }

        return a;
    }

    if (*a < *c) {
        return a;
    }

    if (*b < *c) {
        return c;
    }

    return b;
}

// OFFSET: [0x0077D530<LiquidChunkAndDistance*, LiquidChunkAndDistance>]
template <class RandomIt, class T>
RandomIt blz::internal::unguarded_partition(RandomIt first, RandomIt last, T pivot) {
    while (true) {
        while (*first < pivot) {
            first++;
        }

        last--;

        while (pivot < *last) {
            last--;
        }

        if (!(first < last)) {
            return first;
        }

        typename blz::internal::ValueOf<RandomIt>::Type tmp = *first;
        *first = *last;
        *last = tmp;
        first++;
    }
}

// OFFSET: [0x0077DC50<LiquidChunkAndDistance*>]
template <class RandomIt>
void blz::make_heap(RandomIt first, RandomIt last) {
    int32_t len = last - first;

    if (len < 2) {
        return;
    }

    int32_t parent = (len - 2) / 2;

    while (true) {
        typename blz::internal::ValueOf<RandomIt>::Type value = first[parent];
        blz::internal::adjust_heap(first, parent, len, value);

        if (parent == 0) {
            return;
        }

        parent--;
    }
}

// OFFSET: [0x0077DCF0<LiquidChunkAndDistance*>]
template <class RandomIt>
void blz::sort_heap(RandomIt first, RandomIt last) {
    while (last - first > 1) {
        last--;
        typename blz::internal::ValueOf<RandomIt>::Type value = *last;
        *last = *first;
        blz::internal::adjust_heap(first, 0, static_cast<int32_t>(last - first), value);
    }
}

// OFFSET: [0x0077DE20<LiquidChunkAndDistance*>]
template <class RandomIt>
void blz::partial_sort(RandomIt first, RandomIt middle, RandomIt last) {
    blz::make_heap(first, middle);

    for (RandomIt i = middle; i < last; i++) {
        if (*i < *first) {
            typename blz::internal::ValueOf<RandomIt>::Type value = *i;
            *i = *first;
            blz::internal::adjust_heap(first, 0, static_cast<int32_t>(middle - first), value);
        }
    }

    blz::sort_heap(first, middle);
}

// OFFSET: [0x0077E090<LiquidChunkAndDistance*, int32_t>]
template <class RandomIt, class Size>
void blz::internal::introsort_loop(RandomIt first, RandomIt last, Size depthLimit) {
    while (last - first > 32) {
        if (depthLimit == 0) {
            blz::partial_sort(first, last, last);
            return;
        }

        depthLimit--;
        RandomIt cut = blz::internal::unguarded_partition(first, last, *blz::internal::median(first, first + (last - first) / 2, last - 1));
        blz::internal::introsort_loop(cut, last, depthLimit);
        last = cut;
    }
}

// INLINED
template <class RandomIt>
void blz::sort(RandomIt first, RandomIt last) {
    if (first == last) {
        return;
    }

    int32_t depth = 0;

    for (int32_t n = last - first; n != 1; n >>= 1) {
        depth++;
    }

    blz::internal::introsort_loop(first, last, depth * 2);
    blz::internal::insertion_sort(first, last);
}
