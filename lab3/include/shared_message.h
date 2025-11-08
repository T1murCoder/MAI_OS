#pragma once

#include <semaphore.h>
#include <string>
#include <cstring>
#include <functional>

constexpr size_t MAX_STRING_LEN = 1024;

struct SharedMessageChannel {
    char data[MAX_STRING_LEN];
    size_t length;
    bool finished;
    sem_t sem_write;
    sem_t sem_read;

    void setString(const std::string& str) {
        length = std::min(str.length(), MAX_STRING_LEN - 1);
        std::strncpy(data, str.c_str(), length);
        data[length] = '\n';
    }

    std::string getString() const {
        return std::string(data, length);
    }
};

SharedMessageChannel* createSharedMemory(const char* name); // родительский создает SharedMemory
SharedMessageChannel* openSharedMemory(const char* name); // дочерний открывает SharedMemory
void closeSharedMemory(SharedMessageChannel*& msg, const char* name, bool is_owner);

void sendMessage(SharedMessageChannel* msg, const std::string& str);
void signalFinished(SharedMessageChannel* msg);

bool receiveMessage(SharedMessageChannel* msg, std::function<void(const std::string&)> func);
