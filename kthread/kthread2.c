#include <linux/module.h>
#include <linux/kthread.h> 
// #include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include "kprotocol.h"

#define LOCKS 1


extern struct queue squeue;

static struct task_struct *kthread2;

int consumer(void *item){
    int cpuid = 0; 
    struct queue *d = (struct queue *)item;

    s64 curr_head;
    s64 next_head;
    s64 tmp; 
    u64 i=0;
    uint64_t *p_stamp = (uint64_t*)kzalloc(QUEUE_SIZE*sizeof(uint64_t), GFP_KERNEL);
    uint64_t *c_stamp = (uint64_t*)kzalloc(QUEUE_SIZE*sizeof(uint64_t), GFP_KERNEL);

    spinlock_t interruptLock2;
    unsigned long flags;

    spin_lock_init(&interruptLock2);
    set_current_state(TASK_UNINTERRUPTIBLE);
    cpuid = get_cpu();
    put_cpu();

    // printk(KERN_INFO "DEBUG: In Consumer Initial count=%lld lock=%d\n", rdtscp(), spin_is_locked(&d->lock));
    printk(KERN_INFO "DEBUG: In Consumer Initial Timestamp=%ld \n", rdtscp());
    // spin_lock_irqsave(&interruptLock2, flags);

    // while (!kthread_should_stop()) {
    do{

        // spin_lock_irqsave(&d->lock, flags);
        // p_stamp[i] = d->data[i & (QUEUE_SIZE-1)];
        // c_stamp[i] = rdtscp();
        // i = (i+1) & (QUEUE_SIZE-1);
        // spin_unlock_irqrestore(&d->lock, flags);
        // printk(KERN_INFO "DEBUG: CurrHead = %llu Enqueue: %llu  Dequeue: %llu   Dur: %llu\n", i, p_stamp[i-1], c_stamp[i-1], (c_stamp[i-1]-p_stamp[i-1]));

        curr_head = atomic_long_read(&d->head);  //atomic_load relaxed
        // smp_mb__after_atomic();  //Do we need this barrier gurantee here?
        next_head = (curr_head + 1);
        
        tmp = atomic_long_read_acquire(&d->tail);
        // smp_mb__after_atomic();  //Do we need this barrier gurantee here?
        // if (curr_head == atomic_long_read_acquire(&d->tail)) { //atomic load on tail acquire 
        if (curr_head == tmp) { //atomic load on tail acquire 
                continue;
        }

        p_stamp[i] =  d->data[curr_head & (QUEUE_SIZE-1)];
        c_stamp[i] = rdtscp();
        // i = (i+1) & (QUEUE_SIZE-1); 
        i = (i+1);
        // i = i+1;
        // d->head = next_head; //atomic store release
        // __atomic_store(&d->head, &next_head, __ATOMIC_RELEASE); 
        // atomic_long_fetch_add_release((long)1, &d->head);
        atomic_long_fetch_inc_release(&d->head);
        // atomic_set_release(&d->head, next_head);
        // smp_mb__after_atomic();  //Do we need this barrier gurantee here?
        // printk(KERN_INFO "I=%lld",i);
        // usleep_range(1000000, 1000001);
        // udelay(5);
        printk(KERN_INFO "DEBUG: CurrHead = %llu Enqueue: %llu  Dequeue: %llu   Dur: %llu\n", curr_head, p_stamp[i-1], c_stamp[i-1], (c_stamp[i-1]-p_stamp[i-1]));
    // }
    }while (i<QUEUE_SIZE);
    // spin_unlock_irqrestore(&interruptLock2, flags);

    for (i =0;i< QUEUE_SIZE;i++) {
        printk(KERN_INFO "DEBUG: Enqueue: %llu  Dequeue: %llu   Dur: %llu\n", p_stamp[i], c_stamp[i], (c_stamp[i]-p_stamp[i]));
    }
    kfree(c_stamp);
    kfree(p_stamp);
    p_stamp = NULL;
    c_stamp = NULL; 
    while (!kthread_should_stop()){
        usleep_range(1000000, 1000100);
    }

    printk(KERN_INFO "DEBUG: Exit from consumer\n");

    return 0;
}
 

int kthread_init(void){

    printk(KERN_INFO "DEBUG: Inside consumer Module init\n");
    int err=0;
    int cpuid = get_cpu();
    put_cpu();

    printk(KERN_INFO "DEBUG: consumer Master thread cpu %d\n", cpuid);

    kthread2 = kthread_create(consumer, (void *)&squeue, "two"); 
    if (IS_ERR(kthread2))
    {
        printk(KERN_INFO "ERROR: Cannot create consumer thread\n");
        err = PTR_ERR(kthread2);
        kthread2= NULL;
        return err;
    }
    kthread_bind(kthread2, 38);
    wake_up_process(kthread2);
 

    return 0;
}

void kthread_exit(void){
    kthread_stop(kthread2);
    printk(KERN_EMERG "DEBUG: consumer Module unload successful\n");
}

module_init(kthread_init);
module_exit(kthread_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vikram");