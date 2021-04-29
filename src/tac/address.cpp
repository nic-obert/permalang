#include "tac.hh"


using namespace tac;


std::unordered_map<Value, unsigned int> Address::addresses = std::unordered_map<Value, unsigned int>();


Address::Address()
{
    
}


// returns whether tha address is a temporary address or not
bool Address::isTempAddress(const Value address)
{
    return addresses.find(address) != addresses.end();
}


const Address* Address::getAddress()
{   
    const Address* address = new const Address();
    addresses[toValue(address)] = 0;
    return address;
}


void Address::free(const Address* const address)
{
    delete address;
}


std::ostream& operator<<(std::ostream& stream, const Address* const address)
{
    /* 
        if the address is a temporary address created by Address::getAddress()
        print it as a plain memory address, whereas if it's not, print the 
        string (variable name) it's pointing to
    */

    if (Address::isTempAddress(toValue(address)))
    {
        return stream << "[" << (void*) address << "]";
    }

    return stream << "[" << *((std::string*) address) << "]";
}
    


