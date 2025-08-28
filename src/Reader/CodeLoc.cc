#include "CodeLoc.h"

std::string CodeLoc::print() const {
    return "Line" + std::to_string(line) + "Column" + std::to_string(column);
}