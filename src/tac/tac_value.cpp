#include "tac.hh"


using namespace tac;


TacValue::TacValue()
{

}


TacValue::TacValue(TacValueType type, Value value)
: type(type), value(value)
{

}


std::ostream& operator<<(std::ostream& stream, TacValue const& value)
{
    switch (value.type)
    {
        case TacValueType::ADDRESS:
            // an address' value is a reference to something else
            return stream << (const Address*) value.value;
        case TacValueType::LITERAL:
            // a literal stores its value directly
            return stream << value.value;
        case TacValueType::LABEL:
            // cast to a void* because the Value of a label is a pointer
            return stream << "@[" << (void*) value.value << ']';
    }

    // this code never gets reached
    return stream;
}

