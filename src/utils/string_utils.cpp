#include "utils.hh"


using namespace string_utils;


size_t string_utils::indexOfChar(std::string& string, size_t beginning, char c)
{
    for (; string[beginning] != c; beginning++);
    return beginning;
}


size_t string_utils::indexOfNotChar(std::string& string, size_t beginning, char c)
{
    for (; string[beginning] == c; beginning++);
    return beginning;
}


void string_utils::byteToString(unsigned char number, std::string& output)
{
    // calculate number length
    unsigned char length = 0;
    unsigned char n = number;

    do
    {
        length++;
        n /= 10;
    } while(n != 0);

    output.resize(length + 1, '\0');

    for (n = (unsigned char) (length - 1); number != 0; n--)
    {
        // extract rightmost digit and add to the string
        output[n] = (char) (number % 10 + '0');
        // drop rightmost digit
        number /= 10;
    }

}

