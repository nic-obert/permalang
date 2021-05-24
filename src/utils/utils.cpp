#include "utils.hh"

#include "pch.hh"


const char* charToASCII(unsigned char number)
{
    // calculate number length
    unsigned char length = 0;
    unsigned char n = number;

    do
    {
        length++;
        n /= 10;
    } while(n != 0);


    char* output = (char*) malloc(length + 1);

    for (n = 0; n != length; n++)
    {
        // extract rightmost digit
        output[n] = number % 10 + '0';
        // drop rightmost digit
        number /= 10;
    }

    output[n] = '\0';

    return output;
}

