#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <atomic>

#include <x86intrin.h>

#define NAME "/shmemtest_new"

#define COUNT (4096ULL)

#define NUM (4096ULL)



template <typename T>
struct Data {
    std::atomic<uint64_t> head;
    char pad1[56];
    std::atomic<uint64_t> tail;
    char pad2[56];
    uint64_t size;
    T data[];
};

#define SIZE (sizeof(Data<uint64_t>) + NUM * sizeof(uint64_t))

template <typename T>
T pop(Data<T>* d) {
    do {
        const auto cur_head = d->head.load(std::memory_order_relaxed);
        const auto next_head = cur_head + 1;
        if (cur_head == d->tail.load(std::memory_order_acquire))
            continue;
        T item = d->data[cur_head & (NUM-1)];

        d->head.store(next_head, std::memory_order_release);
        return item;

    } while(true);
}

template <typename T>
void push(Data<T>* d) {
    //unsigned long long int tick;
    unsigned int aux;
    do {
        const auto cur_tail = d->tail.load(std::memory_order_relaxed);
        const auto next_tail = cur_tail + 1;
        if (next_tail != d->head.load(std::memory_order_acquire)) {
            //d->data[cur_tail & (NUM-1)] = item;
            d->data[cur_tail & (NUM-1)] = (uint64_t)__rdtscp(&aux);
            d->tail.store(next_tail, std::memory_order_release);
            return;
        }

    } while(true);
}

template <typename T>
void push(Data<T>* d, const T item) {
    do {
        const auto cur_tail = d->tail.load(std::memory_order_relaxed);
        const auto next_tail = cur_tail + 1;
        if (next_tail != d->head.load(std::memory_order_acquire)) {
            d->data[cur_tail & (NUM-1)] = item;
            d->tail.store(next_tail, std::memory_order_release);
            return;
        }

    } while(true);
}




#endif // __PROTOCOL_H_
