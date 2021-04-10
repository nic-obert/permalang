#pragma once


// a Value can represent anything since it's a pointer
typedef unsigned long Value;


// converts anything to a Value
#define toValue(x) ((Value) x)

