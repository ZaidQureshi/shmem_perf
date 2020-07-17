#include "protocol.h"

#include <atomic>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <x86intrin.h>



int main() {
    char in;
    //unsigned long long int tick;
    unsigned int aux;

    uint64_t* enq_tick = (uint64_t*) malloc(COUNT * sizeof(uint64_t));
    uint64_t* deq_tick = (uint64_t*) malloc(COUNT * sizeof(uint64_t));


    
    int fd = -1;
    while (fd == -1) {
        fd = shm_open(NAME, O_RDWR, 0666);
        if (fd < 0 && errno != ENOENT) {
            perror("shm_open()");
            return -1;
        }
    }

    ftruncate(fd, SIZE);

    struct Data<uint64_t>* data = (struct Data<uint64_t>*)
        mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED | MAP_POPULATE, fd, 0);

    mlockall(0);

    std::cout << "Ready!" << std::endl;
    std::cin >> in;

    for(size_t i = 0; i < COUNT; i++) {
        uint64_t item = pop(data);
        deq_tick[i] = __rdtscp(&aux);
        enq_tick[i] = item;

    }

    for(size_t i = 0; i < COUNT; i++) {
        std::cout << "Enqueue: " << enq_tick[i] << "\tDequeue: " << deq_tick[i] << "\tDuration: " << (deq_tick[i] - enq_tick[i]) << std::endl;
    }

    munlockall();

    free(enq_tick);
    free(deq_tick);

    munmap(data, SIZE);

    close(fd);

}
