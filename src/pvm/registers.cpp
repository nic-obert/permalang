#include "pvm.hh"


using namespace pvm;


void* Pvm::getRegister(Registers reg) const
{
    switch (reg)
    {
    case Registers::RGA:
        return (void*) &rga;
    case Registers::RGB:
        return (void*) &rgb;
    case Registers::RDR:
        return (void*) &rdr;
    case Registers::RZF:
        return (void*) &rzf;
    case Registers::RSF:
        return (void*) &rsf;    
    }

    // this code never gets reached
    return nullptr;
}


// lookup table for Register string representation
static const char* const registerRepr[] =
{
    "rga",
    "rgb",
    "rdr",
    "rzf",
    "rsf"
};


std::ostream& operator<<(std::ostream& stream, const Registers& reg)
{
    return stream << registerRepr[(unsigned char) reg];
}

