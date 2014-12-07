#ifndef ALGORITHM_H
#define ALGORITHM_H

namespace ktl {

template <class Iterator, class Compare>
void bubble_sort(Iterator begin, Iterator end, Compare comp) {
    Iterator last = end;
    while(begin != last) {
        for(Iterator it = begin; it != last; ++it) {
            Iterator next = it;
            ++next;
            if(next != last && comp(*next, *it)) {
                auto tmp = *it;
                *it = *next;
                *next = tmp;
            }
        }
        --last;
    }
    
}

template<typename Iterator, class Compare>
void sort(Iterator begin, Iterator end, Compare comp) {
    bubble_sort(begin, end, comp);
}

template <class Iterator, class T>
Iterator find(Iterator first, Iterator last, const T& value)
{
    while(first != last) {
        if (*first == value)
            break;
        ++first;
    }
    return first;
}
    
}

#endif
