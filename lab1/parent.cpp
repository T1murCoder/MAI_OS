#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <set>


int create_process();

int main() {

    int pipe1[2], pipe2[2]; // 0 - чтение, 1 - запись
    if (-1 == pipe(pipe1) || -1 == pipe(pipe2)) {
        perror("pipe error");
        return -1;
    }

    std::string file1, file2;
    std::cout << "Введите имя файла file1: ";
    std::getline(std::cin, file1);
    std::cout << "Введите имя файла file2: ";
    std::getline(std::cin, file2);

    int fd1 = open(file1.c_str(), O_WRONLY | O_CREAT);
    int fd2 = open(file2.c_str(), O_WRONLY | O_CREAT);


    pid_t pid_1 = create_process();    
    if (0 == pid_1) {
        close(pipe1[1]);
        if (-1 == dup2(pipe1[0], STDIN_FILENO)) {
            perror("dup2 error");
            exit(-1);
        };
        close(pipe1[0]);
        close(pipe2[0]); close(pipe2[1]);

        execl("./child_exe", "./child", std::to_string(fd1).c_str(), NULL);
        perror("execl error");

        exit(1);
    }

    pid_t pid_2 = create_process();
    if (0 == pid_2) {
        close(pipe2[1]);
        if (-1 == dup2(pipe2[0], STDIN_FILENO)) {
            perror("dup2 error");
            exit(-1);
        };
        close(pipe2[0]);
        close(pipe1[0]); close(pipe1[1]);

        execl("./child_exe", "./child", std::to_string(fd2).c_str(), NULL);
        perror("execl error");

        exit(1);
    }
    // Тут уже родитель
    close(pipe1[0]);
    close(pipe2[0]);


    std::string line;
    std::cout << "Вводите строки НА ЛАТИНИЦЕ!!!!!!!!!!!!! (Ctrl+D для завершения):\n";
    while (std::getline(std::cin, line)) {
        line += '\n';
        if (line.size() <= 10 + 1) {
            write(pipe1[1], line.c_str(), line.size());
        }
        else {
            write(pipe2[1], line.c_str(), line.size());
        }
    }
    close(pipe1[1]);
    close(pipe2[1]);

    waitpid(pid_1, nullptr, 0);
    waitpid(pid_2, nullptr, 0);

    close(fd1);
    close(fd2);
    
    return 0;
}

int create_process() {
    pid_t pid = fork();
    if (-1 == pid)
    {
        perror("fork error");
        exit(-1);
    }
    return pid;
}