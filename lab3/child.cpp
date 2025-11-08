#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include "include/shared_message.h"

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

void processMessage(const std::string& message, const char* child_name) {
    std::string result = remove_vowels(message);

    printf("[child %s] %s\n", child_name, result.c_str());
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Использование: " << argv[0] << " <тут дескриптор файла> <тут имя SharedMemory>\n";
        return 1;
    }
    const char* SHM_NAME = argv[2];

    int fd = std::stoi(argv[1]);
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2 error");
        close(fd);
        exit(-1);
    }
    close(fd);

    SharedMessageChannel* msg = openSharedMemory(SHM_NAME);

    while (receiveMessage(
            msg,
            [SHM_NAME](const std::string& message) {processMessage(message, SHM_NAME);}
    )) {}

    closeSharedMemory(msg, SHM_NAME, false);

    return 0;
}