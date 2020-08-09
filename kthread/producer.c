#include <linux/module.h>
#include <linux/kthread.h> 
// #include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
// #include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/slab.h>

#include "kprotocol.h"

#define LOCKS 1

// https://stackoverflow.com/questions/25446203/how-to-export-a-struct-between-two-kernel-modules-using-export-symbol-or-equival

struct queue squeue; 
// struct queue *sqptr = &squeue; 
EXPORT_SYMBOL(squeue);

static struct task_struct *kproducer;

int producer(void *item){
    int cpuid = 0; 
    volatile u64 j=0;
    struct queue *d = (struct queue*)item;

    s64 curr_tail; //kernel only has signed implementation.
    s64 next_tail;
    uint64_t aux;
    s64 tmp; 

    spinlock_t interruptLock;
    unsigned long flags;

    spin_lock_init(&interruptLock);
    set_current_state(TASK_UNINTERRUPTIBLE);
    cpuid = get_cpu();
    put_cpu();

    // printk(KERN_INFO "DEBUG: In Producer Initial count=%lld lock=%d\n", rdtscp(), spin_is_locked(&d->lock));
    printk(KERN_INFO "DEBUG: In Producer Initial Timestamp=%ld \n", rdtscp());

    atomic_long_set(&d->head, 0); //set the counts to 0.
    atomic_long_set(&d->head, 0);
    smp_mb__after_atomic(); //barrier

    d->data = (uint64_t*)kmalloc((QUEUE_SIZE*sizeof(uint64_t)), GFP_ATOMIC);  //__GFP_DMA
    if(!d->data)
        printk(KERN_ERR "DEBUG: Not enough memory avaialble\n");
    // spin_lock_irqsave(&interruptLock, flags);

    while (!kthread_should_stop()) {
    // do{
        // spin_lock_irqsave(&d->lock, flags);
        // d->data[i & (QUEUE_SIZE-1)] = rdtscp();
        // i = (i+1) & (QUEUE_SIZE-1);
        // spin_unlock_irqrestore(&d->lock, flags);

        curr_tail = atomic_long_read(&d->tail); //atomic_load relaxed
        // smp_mb__after_atomic();  //Do we need this barrier gurantee here?
        next_tail = (curr_tail + 1);
        tmp = atomic_long_read_acquire(&d->head);
        // smp_mb__after_atomic();  //Do we need this barrier gurantee here?
        // if (next_tail != atomic_long_read_acquire(&d->head)) {//atomic _load on head acquire 
        if (next_tail != tmp) {//atomic _load on head acquire 
            aux = rdtscp();
            d->data[curr_tail & (QUEUE_SIZE-1)] = aux;
            // d->tail = next_tail; //atomic store release
            // atomic_long_fetch_add_release((long)1, &d->tail);
            atomic_long_fetch_inc_release(&d->tail);
            // atomic_set_release(&d->tail, next_tail);
            // smp_mb__after_atomic();  //Do we need this barrier gurantee here?
            // printk(KERN_INFO "DEBUG: Enqueue curr_tail = %llu val: %llu\n", curr_tail, aux);
            // // return;
        }
        
        // usleep_range(1000000, 1000001);
        for (j = 0; j < 1000000000ULL; j++);
    }
    // spin_unlock_irqrestore(&interruptLock, flags);
    printk(KERN_INFO "DEBUG: Exit from Producer\n");

    kfree(d->data);
    d->data = NULL; 

    return 0;
}
 

int kthread_init(void){

    printk(KERN_INFO "DEBUG: Inside Producer Module init\n");
    int err=0;
    int cpuid = get_cpu();
    put_cpu();

    printk(KERN_INFO "DEBUG: Producer Master thread cpu %d\n", cpuid);

    kproducer = kthread_create(producer, (void *)&squeue, "one"); 
    if (IS_ERR(kproducer))
    {
        printk(KERN_INFO "ERROR: Cannot create Producer thread\n");
        err = PTR_ERR(kproducer);
        kproducer = NULL;
        return err;
    }
    kthread_bind(kproducer, 37);
    wake_up_process(kproducer);
 

    return 0;
}

void kthread_exit(void){
    kthread_stop(kproducer);
    printk(KERN_EMERG "DEBUG: Producer Module unload successful\n");
}

module_init(kthread_init);
module_exit(kthread_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vikram");