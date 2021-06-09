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
    return *(long*) (stack + address);
}


bool Memory::getBit(Address address) const
{
    return *(bool*) (stack + address);
}


int Memory::getInt(Address address) const
{
    return *(int*) (stack + address);
}


void Memory::set(Address address, Byte value)
{
    stack[address] = value;
}


void Memory::set(Address address, long value)
{
    *(long*) (stack + address) = value;
}


void Memory::set(Address address, bool value)
{
    // do not fragment the stack in chunks smaller than 1 byte
    // thus booleans (1 bit) are approximated to 1 byte
    stack[address] = value;
}


void Memory::set(Address address, int value)
{
    *(int*) (stack + address) = value;
}

