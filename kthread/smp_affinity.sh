#!/bin/bash
for f in /proc/irq/*/smp_affinity;
do
    #value=`cat $f`
    value2=`sed 's/^./0/g' $f`
    echo "$value2" > $f
done

echo 0 > /proc/sys/kernel/hung_task_timeout_secs
