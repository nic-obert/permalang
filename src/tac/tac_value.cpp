#include "tac.hh"
#include "utils.hh"

#include <iostream>


using namespace tac;


TacValue::TacValue(TacValueType type, Value value)
: type(type), value(value)
{
    
}


std::ostream& operator<<(std::ostream& stream, TacValue const& value)
{
    switch (value.type)
    {
        case TacValueType::ADDRESS:
            return stream << (const Address*) value.value;
        case TacValueType::LITERAL:
            return stream << value.value;
        case TacValueType::LABEL:
            return stream << "@[" << (void*) value.value << ']';
    }
}


std::ostream& operator<<(std::ostream& stream, TacValue const* value)
{
    return stream << *value;
}


