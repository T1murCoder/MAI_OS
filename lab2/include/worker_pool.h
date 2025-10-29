#pragma once

#include <queue>
#include <vector>
#include <pthread.h>

struct Task {
    void (*function)(void*);
    void* arg;
};

class WorkerPool {
public:
    explicit WorkerPool(size_t num_threads);
    ~WorkerPool();
    
    void enqueue(void (*func)(void*), void* arg);
    void wait();

private:
    static void* worker_wrapper(void* arg);
    void worker();

    std::vector<pthread_t> threads_;
    std::queue<Task> tasks_;
    
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    pthread_cond_t finished_cond_;
    
    bool stop_;
    size_t active_tasks_;
};
