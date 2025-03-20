#include <string>
#include "utf8.hh"

size_t utf8_length(const std::string &str)
{
    size_t length = 0;
    for (auto it = str.begin(); it != str.end();)
    {
        unsigned char c = *it;
        if ((c & 0x80) == 0x00)
        {
            // 1-byte character
            ++it;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            // 2-byte character
            it += 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            // 3-byte character
            it += 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            // 4-byte character
            it += 4;
        }
        ++length;
    }
    return length;
}

std::string utf8_char_at(const std::string &str, size_t n)
{
    size_t current_pos = 0;
    size_t byte_index = 0;

    while (current_pos < n && byte_index < str.size())
    {
        unsigned char c = str[byte_index];
        if ((c & 0x80) == 0x00)
        {
            // 1-byte character
            ++byte_index;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            // 2-byte character
            byte_index += 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            // 3-byte character
            byte_index += 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            // 4-byte character
            byte_index += 4;
        }
        ++current_pos;
    }

    if (current_pos == n && byte_index < str.size())
    {
        unsigned char c = str[byte_index];
        if ((c & 0x80) == 0x00)
        {
            // 1-byte character
            return str.substr(byte_index, 1);
        }
        else if ((c & 0xE0) == 0xC0)
        {
            // 2-byte character
            return str.substr(byte_index, 2);
        }
        else if ((c & 0xF0) == 0xE0)
        {
            // 3-byte character
            return str.substr(byte_index, 3);
        }
        else if ((c & 0xF8) == 0xF0)
        {
            // 4-byte character
            return str.substr(byte_index, 4);
        }
    }

    // Return an empty string if the position is out of bounds
    return "";
}

std::string utf8_char_at(const std::string &str, size_t n, std::string defaultVal)
{
    std::string ret = utf8_char_at(str, n);
    if (ret.empty())
        return defaultVal;
    return ret;
}

std::string replaceChar(std::string s, char c1, char c2)
{
    for (long unsigned int i = 0; i < s.length(); i++)
    {
        if (s[i] == c1)
            s[i] = c2;
        // else if (s[i] == c2)
        //     s[i] = c1;
    }
    return s;
}

std::string deleteChar(std::string s, char c1)
{
    std::string ret("");
    for (long unsigned int i = 0; i < s.length(); i++)
    {
        if (s[i] != c1)
            ret += s[i];
    }
    return ret;
}