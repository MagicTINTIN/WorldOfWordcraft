#pragma once
#include <string>

size_t utf8_length(const std::string &str);

std::string utf8_char_at(const std::string &str, size_t n);

std::string utf8_char_at(const std::string &str, size_t n, std::string defaultVal);

std::string replaceChar(std::string s, char c1, char c2);

std::string deleteChar(std::string s, char c1);
