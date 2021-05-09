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


std::ostream& operator<<(std::ostream& stream, const Registers& reg)
{
    switch (reg)
    {
    case Registers::RDR:
        stream << "rdr";
        break;
    
    case Registers::RGA:
        stream << "rga";
    
    case Registers::RGB:
        stream << "rgb";

    case Registers::RSF:
        stream << "rsf";
    
    case Registers::RZF:
        stream << "rzf";
        
    }

    return stream;
}

