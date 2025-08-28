#include "Reader.h"
#include <cassert>

Reader::Reader(const std::filesystem::path &filePath){
    input = std::ifstream(filePath);
    if (!input.is_open())
        throw std::runtime_error("Reader error: Source file could not be read");

    advance();
};
Reader::~Reader(){
    input.close();
};
char Reader::getChar() const {
    return currenChar;
}

CodeLoc Reader::getCodeLoc() const{
    return curCodeLoc;
}

bool Reader::isEOF() const {
    return !moreToRead;
}

void Reader::advance(){
    assert(!isEOF());
    if (!input.get(currenChar))
        moreToRead = false;
    if (currenChar == '\n') {
        curCodeLoc.line++;
        curCodeLoc.column = 0;
    }
    curCodeLoc.column++;
}
void Reader::expect(char val){
    advance();
    assert(currenChar = val);
}