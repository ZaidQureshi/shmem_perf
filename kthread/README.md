# Kernel Thread Communication Microbenchmark
Lock free datastruct based producer consumer implementation in the kernel space between the two kernel modules. The code can be ported to single kernel module if needed. 

Usage:

```
make 

#insert
sudo insmod producer.ko && sudo insmod consumer.ko
dmesg -wH

#remove
sudo rmmod consumer.ko 
sudo rmmod producer.ko
```

Consumer has dependency on producer and requires to be removed before producer. This is because of the `EXPORT_SYMBOL` call used to communicate between the two kernel modules. 
