#include "../include/shared_message.h"
#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>


SharedMessageChannel* createSharedMemory(const char* name) {
    shm_unlink(name); // Удаляем, если он уже вдруг существует

    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // FD_CLOEXEC указывать не обязательно, т.к. он по дефолту здесь установлен
    if (shm_fd == -1) {
        throw std::runtime_error("shm_open failed");
    }

    if (ftruncate(shm_fd, sizeof(SharedMessageChannel)) == -1) {
        close(shm_fd);
        shm_unlink(name);
        throw std::runtime_error("ftruncate failed");

    }

    void* addr = mmap(nullptr, sizeof(SharedMessageChannel), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd); // Закрываем файловый дескриптор после применения отображения, т.к. в мануале написано, что можно (https://man7.org/linux/man-pages/man2/mmap.2.html)

    if (addr == MAP_FAILED) {
        shm_unlink(name);
        throw std::runtime_error("mmap failed");
    }

    SharedMessageChannel* msg = static_cast<SharedMessageChannel*>(addr);

    // Родительский сразу готов писать
    if (sem_init(&msg->sem_write, 1, 1) == -1) {
        munmap(msg, sizeof(SharedMessageChannel));
        shm_unlink(name);
        throw std::runtime_error("sem_init sem_write failed");
    }
    
    // Дочерний будет висеть на семафоре
    if (sem_init(&msg->sem_read, 1, 0) == -1) {
        sem_destroy(&msg->sem_write);
        munmap(msg, sizeof(SharedMessageChannel));
        shm_unlink(name);
        throw std::runtime_error("sem_init sem_read failed");
    }

    msg->finished = false;
    msg->length = 0;
    
    return msg;
}

SharedMessageChannel* openSharedMemory(const char* name) {
    int shm_fd = shm_open(name, O_RDWR, 0666);
    if (shm_fd == -1) {
        throw std::runtime_error("shm_open failed");
    }

    void* addr = mmap(nullptr, sizeof(SharedMessageChannel), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    if (addr == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }

    return static_cast<SharedMessageChannel*>(addr);
}

void closeSharedMemory(SharedMessageChannel*& msg, const char* name, const bool is_owner) {
    if (msg != nullptr) {
        if (is_owner) {
            sem_destroy(&msg->sem_write);
            sem_destroy(&msg->sem_read);
        }

        munmap(msg, sizeof(SharedMessageChannel));
        msg = nullptr; // зануляем, чтобы нельзя было использовать невалидный указатель
    }

    if (is_owner) {
        shm_unlink(name);
    }
}

void sendMessage(SharedMessageChannel* msg, const std::string& str) {
    sem_wait(&msg->sem_write);
    msg->setString(str);
    sem_post(&msg->sem_read);
}

void signalFinished(SharedMessageChannel* msg) {
    sem_wait(&msg->sem_write);
    msg->finished = true;
    sem_post(&msg->sem_read);
}

bool receiveMessage(SharedMessageChannel* msg, std::function<void(const std::string&)> func) {
    sem_wait(&msg->sem_read);

    bool should_continue = !msg->finished;

    if (should_continue) {
        func(msg->getString());
    }
    sem_post(&msg->sem_write);

    return should_continue;
}