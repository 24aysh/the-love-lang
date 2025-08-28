#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include "CodeLoc.h"

using namespace std;
class Reader{
    
public:
    explicit Reader(const std::filesystem::path &filePath);
    ~Reader();
    char getChar() const;
    CodeLoc getCodeLoc() const;
    void advance();
    void expect(char);
    bool isEOF() const;
private:
    ifstream input;
    char currenChar = '\0';
    bool moreToRead = true;
    CodeLoc curCodeLoc{1,0};

};