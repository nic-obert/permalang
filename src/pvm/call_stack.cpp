#include "pvm.hh"


using namespace pvm;


CallStack::CallStack(Address address, CallStack* prev)
: address(address), prev(prev)
{

}

