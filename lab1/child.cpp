#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>

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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Вот так используй: " << argv[0] << " <тут файл куда выводить>\n";
        return 1;
    }

    int fd = std::stoi(argv[1]);
    if (-1 == dup2(fd, STDOUT_FILENO)) {
            perror("dup2 error");
            exit(-1);
        };
    close(fd);

    std::string line;
    while (std::getline(std::cin, line)) {
        std::string result = remove_vowels(line);

        printf("[child %d] %s\n", fd, result.c_str());
        fflush(stdout);
    }

    return 0;
}