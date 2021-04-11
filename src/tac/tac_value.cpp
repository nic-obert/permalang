#include "tac.hh"
#include "utils.hh"

#include <iostream>


using namespace tac;


TacValue::TacValue(bool isAddress, Value value)
: isAddress(isAddress), value(value)
{
    
}


std::ostream& operator<<(std::ostream& stream, TacValue const& value)
{
    if (value.isAddress)
    {
        return stream << (const Address*) value.value;
    }
    
    return stream << value.value;
}


std::ostream& operator<<(std::ostream& stream, TacValue const* value)
{
    return stream << *value;
}


