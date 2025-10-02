#include <iostream>
#include <string>
#include <cstdio>

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

    const char* file_name = argv[1];

    FILE* fout = fopen(file_name, "wb");
    if (!fout) {
        perror("fopen error");
        return 1;
    }

    std::string line;
    while (std::getline(std::cin, line)) {
        std::string result = remove_vowels(line);
        fwrite(result.c_str(), sizeof(char), result.size(), fout);
        fwrite("\n", sizeof(char), 1, fout);
        std::cout << "[child " << file_name << "] " << result << std::endl;
    }

    fclose(fout);
    return 0;
}