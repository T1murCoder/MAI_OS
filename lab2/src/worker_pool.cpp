#include "../include/worker_pool.h"
#include <stdexcept>
#include <string>

WorkerPool::WorkerPool(size_t num_threads) : stop_(false), active_tasks_(0) {
        
    if (pthread_mutex_init(&mutex_, nullptr) != 0) {
        throw std::runtime_error(std::string("pthread_mutex_init failed"));
    };
    if (pthread_cond_init(&cond_, nullptr) != 0) {
        throw std::runtime_error(std::string("pthread_cond_init failed"));
    };
    if (pthread_cond_init(&finished_cond_, nullptr) != 0) {
        throw std::runtime_error(std::string("pthread_cond_init failed"));
    };

    threads_.resize(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        int err = pthread_create(&threads_[i], nullptr, worker_wrapper, this);
        if (err != 0) throw std::runtime_error(std::string("pthread_create failed"));
    }
}

WorkerPool::~WorkerPool() {
    pthread_mutex_lock(&mutex_);
    stop_ = true;
    pthread_mutex_unlock(&mutex_);
    
    pthread_cond_broadcast(&cond_);

    for (pthread_t& t : threads_) {
        pthread_join(t, nullptr);
    }

    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&cond_);
    pthread_cond_destroy(&finished_cond_);
}

void WorkerPool::enqueue(void (*func)(void*), void* arg) {
    pthread_mutex_lock(&mutex_);
    tasks_.push({func, arg});
    pthread_mutex_unlock(&mutex_);
    pthread_cond_signal(&cond_);
}

void WorkerPool::wait() {
    pthread_mutex_lock(&mutex_);
    while (!tasks_.empty() || active_tasks_ > 0) {
        pthread_cond_wait(&finished_cond_, &mutex_);
    }
    // started_ = false;
    pthread_mutex_unlock(&mutex_);
}

void* WorkerPool::worker_wrapper(void* arg) {
    static_cast<WorkerPool*>(arg)->worker();
    return nullptr;
}

void WorkerPool::worker() {

    while (true) {
        pthread_mutex_lock(&mutex_); // залочили, чтобы к очереди не было одновременного доступа
        
        while (tasks_.empty() && !stop_) {
            pthread_cond_wait(&cond_, &mutex_);
        }

        if (stop_ && tasks_.empty()) {
            pthread_mutex_unlock(&mutex_);
            break;
        }

        Task task = tasks_.front();
        tasks_.pop();
        ++active_tasks_;
        pthread_mutex_unlock(&mutex_);

        task.function(task.arg);

        pthread_mutex_lock(&mutex_);
        --active_tasks_;
        if (tasks_.empty() && active_tasks_ == 0) {
            pthread_cond_signal(&finished_cond_);
        }
        pthread_mutex_unlock(&mutex_);
    }
}