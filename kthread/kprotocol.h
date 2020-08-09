#ifndef __KPROTOCOL_H__
#define __KPROTOCOL_H__

// #ifndef __ATOMIC_H__
// #define __ATOMIC_H__

#include <linux/atomic.h>
// #include <linux/atomic-long.h>
// #include <linux/barrier.h>
// #include <linux/cmpxchg.h>
// #include <x86intrin.h>
#include <asm-generic/barrier.h>
#include <linux/types.h>
#include <linux/spinlock.h>

#define QUEUE_SIZE 4096ULL

static inline unsigned long rdtscp(void)
{
    unsigned long var;
    // unsigned int hi, lo;

    __asm__ __volatile__("rdtscp; "         // serializing read of tsc
        "shl $32,%%rdx; "  // shift higher 32 bits stored in rdx up
        "or %%rdx,%%rax"   // and or onto rax
        : "=a"(var)        // output to tsc variable
        :
        : "%rcx", "%rdx"); // rcx and rdx are clobbered

    // mb();
    // var = ((unsigned long)hi << 32) | lo;
    return (var);
}



struct queue{
    atomic_long_t head;
    char pad1[56];
    atomic_long_t tail;
    char pad2[56];
    uint64_t count;
    spinlock_t lock;
    uint64_t *data;
} __attribute__((aligned(64)));

extern struct queue squeue;

// #define NAME "/shmemtest_new"
// #define COUNT (4096ULL)
// #define NUM (4096ULL)

// #define SIZE (sizeof(struct queue) + NUM * sizeof(uint64_t))


// void push(struct queue* d)
// {
//     uint64_t cur_tail;
//     uint64_t next_tail;
//     unsigned long aux;
//     do
//     {
//         spin_lock(&d->lock);
//         cur_tail = d->tail;
//         next_tail = cur_tail + 1;
//         if (next_tail != d->head)
//         {
//             aux = rdtscp();
//             d->data[cur_tail & (NUM - 1)] = (uint64_t) aux;
//             d->tail = next_tail;
//             printk(KERN_INFO "INFO: Enqueue Success curtail:%llu  data: %lu\n", cur_tail, aux);
//             break;
//         }
//         spin_unlock(&d->lock);

//     } while (true);
// }

// uint64_t pop(struct queue* d)
// {
//     uint64_t cur_head; 
//     uint64_t next_head; 
//     uint64_t item; 
//     do
//     {
//         spin_lock(&d->lock);
//         cur_head = d->head;
//         next_head = cur_head + 1;
//         if (cur_head == d->tail)
//             continue;
//         item = d->data[cur_head & (NUM - 1)];

//         d->head = next_head;
//         printk(KERN_INFO "INFO: Dequeue Success curhead:%llu  data: %llu\n", cur_head, item);
//         spin_unlock(&d->lock);
//         return item;

//     } while (true);
// }

 

#endif /*__KPROTOCOL_H__*/