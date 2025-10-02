#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <set>


int create_process();

const std::string vowels = "aeiouyAEIOUY";

bool is_vowel(char c) {
    return vowels.find(c) != std::string::npos;
}

std::string remove_vowels(const std::string& s) {
    std::string result;
    for (char c : s) {
        if (!is_vowel(c)) result += c;
    }
    return result;
}

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

    pid_t pid_1 = create_process();    
    if (0 == pid_1) { // это дочерний процесс
        close(pipe1[1]);
        if (-1 == dup2(pipe1[0], STDIN_FILENO)) {
            perror("dup2 error");
            exit(-1);
        };
        close(pipe1[0]);
        close(pipe2[0]); close(pipe2[1]);

        // FILE* fout = fopen(file1.c_str(), "wb");
        // if (!fout) {perror("fopen"); return -1;}

        // std::string line;
        // while(std::getline(std::cin, line)) {
        //     std::string result = remove_vowels(line);
        //     fwrite(result.c_str(), sizeof(char), result.size(), fout);
        //     fwrite("\n", sizeof(char), 1, fout);
        //     std::cout << "[child 1] " << result << std::endl;
        // }

        execl("./child_exe", "./child", file1.c_str(), NULL);
        perror("execl error");

        // fclose(fout);
        exit(1);
    }

    pid_t pid_2 = create_process();
    if (0 == pid_2) { // это дочерний процесс
        close(pipe2[1]);
        if (-1 == dup2(pipe2[0], STDIN_FILENO)) {
            perror("dup2 error");
            exit(-1);
        };
        close(pipe2[0]);
        close(pipe1[0]); close(pipe1[1]);

        // FILE* fout = fopen(file2.c_str(), "wb");
        // if (!fout) {perror("fopen"); return -1;}

        // std::string line;
        // while(std::getline(std::cin, line)) {
        //     std::string result = remove_vowels(line);
        //     fwrite(result.c_str(), sizeof(char), result.size(), fout);
        //     fwrite("\n", sizeof(char), 1, fout);
        //     std::cout << "[child 2] " << result << std::endl;
        // }

        execl("./child_exe", "./child", file2.c_str(), NULL);
        perror("execl error");

        // fclose(fout);
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