#pragma once

#include <queue>
#include <vector>
#include <pthread.h>

struct Task {
    void (*function)(void*);
    void* arg;
};

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();
    
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
