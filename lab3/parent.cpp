#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <set>
#include <semaphore.h>
#include <sys/mman.h>
#include "include/shared_message.h"

constexpr const char* SHM_1_NAME = "parent_child_shm_1";
constexpr const char* SHM_2_NAME = "parent_child_shm_2";

int fork_process();

int main() {

    std::string file1, file2;
    std::cout << "Введите имя файла file1: ";
    std::getline(std::cin, file1); // сделать обработку ошибок???
    std::cout << "Введите имя файла file2: ";
    std::getline(std::cin, file2);

    int fd1 = open(file1.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd1 == -1) {
        perror("open file1 error");
        return -1;
    }
    int fd2 = open(file2.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd2 == -1) {
        perror("open file2 error");
        close(fd1);
        return -1;
    }

    SharedMessageChannel* msg_1 = createSharedMemory(SHM_1_NAME);
    SharedMessageChannel* msg_2 = createSharedMemory(SHM_2_NAME);


    pid_t pid_1 = fork_process();    
    if (pid_1 == 0) {

        std::string fd1_str = std::to_string(fd1);
        execl("./child_exe", "./child", fd1_str.c_str(), SHM_1_NAME, nullptr);
        perror("execl error");

        exit(1);
    }

    pid_t pid_2 = fork_process();
    if (pid_2 == 0) {

        std::string fd2_str = std::to_string(fd2);
        execl("./child_exe", "./child", fd2_str.c_str(), SHM_2_NAME, nullptr);
        perror("execl error");

        exit(1);
    }
    // Тут уже родитель


    std::string line;
    std::cout << "Вводите строки НА ЛАТИНИЦЕ!!!!!!!!!!!!! (Ctrl+D для завершения):\n";
    while (std::getline(std::cin, line)) {

        if (line.size() <= 10) {
            sendMessage(msg_1, line);
        } else {
            sendMessage(msg_2, line);
        }
    }
    signalFinished(msg_1);
    signalFinished(msg_2);

    if (waitpid(pid_1, nullptr, 0) == -1) {
        perror("waitpid error");
    };
    if (waitpid(pid_2, nullptr, 0) == -1) {
        perror("waitpid error");
    };

    close(fd1);
    close(fd2);

    closeSharedMemory(msg_1, SHM_1_NAME, true);
    closeSharedMemory(msg_2, SHM_2_NAME, true);
    
    return 0;
}

int fork_process() {
    pid_t pid = fork();
    if (-1 == pid)
    {
        perror("fork error");
        exit(-1);
    }
    return pid;
}