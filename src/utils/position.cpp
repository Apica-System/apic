#include "utils/position.hpp"
#include <iostream>

using namespace utils;

Position::Position()
    : ascii_start(0), ascii_length(0), utf8_start(0), utf8_length(0) {

}

Position::Position(uint64_t ascii_start, uint64_t ascii_length, uint64_t utf8_start, uint64_t utf8_length) {
    this->ascii_start = ascii_start;
    this->ascii_length = ascii_length;
    this->utf8_start = utf8_start;
    this->utf8_length = utf8_length;
}

Position::Position(const Position &from, const Position &to) {
    this->ascii_start = from.ascii_start;
    this->ascii_length = to.getAsciiEnd() - from.ascii_start;
    this->utf8_start = from.utf8_start;
    this->utf8_length = to.getUtf8End() - from.utf8_start;
}

uint64_t Position::getAsciiStart() const {
    return this->ascii_start;
}

uint64_t Position::getAsciiLength() const {
    return this->ascii_length;
}

uint64_t Position::getAsciiEnd() const {
    return this->ascii_start + this->ascii_length;
}

uint64_t Position::getUtf8Start() const {
    return this->utf8_start;
}

uint64_t Position::getUtf8Length() const {
    return this->utf8_length;
}

uint64_t Position::getUtf8End() const {
    return this->utf8_start + this->utf8_length;
}

void Position::show(char end) const {
    std::cout << "Position<ascii: " << this->ascii_start << '.' << this->ascii_length << ", utf8: " << this->utf8_start << '.' << this->utf8_length << '>' << end;
}