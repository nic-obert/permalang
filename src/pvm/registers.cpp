#include "pvm.hh"


using namespace pvm;


void* Pvm::getRegister(Registers reg) const
{
    switch (reg)
    {
    case Registers::GENERAL_A:
        return (void*) &rGeneralA;
    case Registers::GENERAL_B:
        return (void*) &rGeneralB;
    case Registers::DIVISION_REMAINDER:
        return (void*) &rDivisionRemainder;
    case Registers::ZERO_FLAG:
        return (void*) &rZeroFlag;
    case Registers::SIGN_FLAG:
        return (void*) &rSignFlag;
    case Registers::RESULT:
        return (void*) &rResult;   
    }

    // this code never gets reached
    return nullptr;
}


// lookup table for Register string representation
static const char* const registerRepr[] =
{
    "A",
    "B",
    "DIVISION REMAINDER",
    "RESULT",
    "ZERO FLAG",
    "SIGN FLAG",
};


const char* pvm::registerName(Registers reg)
{
    return registerRepr[(unsigned char) reg];
}


std::ostream& operator<<(std::ostream& stream, const Registers& reg)
{
    return stream << '<' << registerRepr[(unsigned char) reg] << '>';
}

