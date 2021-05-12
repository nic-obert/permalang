#include "tac.hh"


using namespace tac;


// lookup table for representation of Tac operations
const char* const tacOpRepr[] = 
{
    "label",
    "if",
    "jump",
    "=",
    "+",
    "-",
    "*",
    "/",
    "==",
    "<",
    "push",
    "pop",
    "NO OP"
};


std::ostream& operator<<(std::ostream& stream, TacOp const& op)
{
    return stream << tacOpRepr[(unsigned char) op];
}


