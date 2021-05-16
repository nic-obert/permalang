#include "pvm.hh"


using namespace pvm;


Memory::Memory(size_t size)
: size(size)
{
    stack = new Byte[size];
}


Memory::~Memory()
{
    delete[] stack;
}


Byte Memory::getByte(Address address) const
{
    return stack[address];
}


long Memory::getLong(Address address) const
{
    return *((long*) (stack + address));
}


bool Memory::getBit(Address address) const
{
    return *((bool*) (stack + address));
}


void Memory::set(Address address, Byte value)
{
    stack[address] = value;
}


void Memory::set(Address address, long value)
{
    *((long*) (stack + address)) = value;
}


void Memory::set(Address address, bool value)
{
    *((bool*) (stack + address)) = value;
}

