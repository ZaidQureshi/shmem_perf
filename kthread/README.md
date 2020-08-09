# Kernel Thread Communication Microbenchmark
Lock free datastruct based producer consumer implementation in the kernel space between the two kernel modules. The code can be ported to single kernel module if needed. 

## Prerequisitive

Following kernel parameters may be required to be passed to the kernel to make this work. (Note: some can be removed but we used all of these) 
```
BOOT_IMAGE=(hd0,msdos1)/vmlinuz-5.6.3 root=/dev/mapper/cl-root ro cma=64G nvidia.NVreg_RestrictProfilingToAdminUsers=0 isolcpus=managed_irq,nohz,domain,36,37,38,39 rcu_nocbs=36,37,38,39 nohz_full=36,37,38,39 rcu_nocb_poll nosoftlockup nowatchdog nmi_watchdog=0
```

## Usage:
```
make 

#set affinity across smps
sudo bash smp_affinity.sh 

#insert
sudo insmod producer.ko && sudo insmod consumer.ko
dmesg -wH

#remove
sudo rmmod consumer.ko 
sudo rmmod producer.ko
```

Consumer has dependency on producer and requires to be removed before producer. This is because of the `EXPORT_SYMBOL` call used to communicate between the two kernel modules. 
