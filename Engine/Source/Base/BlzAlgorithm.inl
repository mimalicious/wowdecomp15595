// OFFSET: [0x00736680<DDMaterial**, int32_t, DDMaterial*, DDMaterialSort>]
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

// OFFSET: [0x00736ED0<DDMaterial**, int32_t, DDMaterial*, DDMaterialSort>]
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

// OFFSET: [0x00736500<DDMaterial**, DDMaterialSort>]
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

// OFFSET: [0x00736570<DDMaterial**, DDMaterialSort>]
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

// OFFSET: [0x00736E50<DDMaterial**, DDMaterial*, DDMaterialSort>]
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

// OFFSET: [0x00737F20<DDMaterial**, DDMaterialSort>]
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

// OFFSET: [0x00737F90<DDMaterial**, DDMaterialSort>]
template <class RandomIt, class Compare>
void blz::sort_heap(RandomIt first, RandomIt last, Compare comp) {
    while (last - first > 1) {
        last--;
        typename blz::internal::ValueOf<RandomIt>::Type value = *last;
        *last = *first;
        blz::internal::adjust_heap(first, 0, static_cast<int32_t>(last - first), value, comp);
    }
}

// OFFSET: [0x007393E0<DDMaterial**, DDMaterialSort>]
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

// OFFSET: [0x00739520<DDMaterial**, int32_t, DDMaterialSort>]
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

// INLINED
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
