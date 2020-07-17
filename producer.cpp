#include "protocol.h"

#include <atomic>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <x86intrin.h>


#define MIN 1
#define MAX 3

inline uint64_t get_loop_iters(void) {
    return (rand() % MAX) + MIN;
}

int main() {
    char in;
    //unsigned long long int tick;
    //unsigned int aux;
    srand((unsigned) time(0));

    int fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);

    if (fd < 0) {
        perror("shm_open()");
        return -1;
    }

    ftruncate(fd, SIZE);

    struct Data<uint64_t>* data = (struct Data<uint64_t>*)
        mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED | MAP_POPULATE, fd, 0);

    data->head = 0;
    data->tail = 0;

    memset(data->data, 0, NUM * sizeof(uint64_t));

    mlockall(0);
    std::cout << "Ready!" << std::endl;
    std::cin >> in;

    for(size_t i = 0; i < COUNT; i++) {
        //tick = __rdtscp(&aux);
        //push(data, (uint64_t)tick);
        push(data);
        for (size_t j = 0; j < 10000000ULL; j++);
        //std::cout << "Enqueue: " << tick << std::endl;
        //uint64_t sleep_count = get_loop_iters();
        //for (size_t j = 0; j < iter_count; j++);
        //sleep(sleep_count);
    }

    munlockall();
    munmap(data, SIZE);

    close(fd);

    shm_unlink(NAME);

    return 0;
    
}
