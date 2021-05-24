#pragma once


// a Value can represent anything since it can be
// either a literal or a pointer
typedef unsigned long Value;


// casts anything to a Value
#define toValue(x) ((Value) x)


const char* charToASCII(unsigned char number);

