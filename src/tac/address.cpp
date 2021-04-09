#include "tac.hh"


using namespace tac;


Address::Address()
{
    
}


const Address* Address::getAddress()
{
    return new const Address();
}


void Address::free(const Address* address)
{
    delete address;
}


std::ostream& operator<<(std::ostream& stream, Address const& address)
{
    return stream << "[" << address << "]";
}


