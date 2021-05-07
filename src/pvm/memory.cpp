#include "pvm.hh"


using namespace pvm;


Memory::Memory(size_t size)
: size(size)
{
    memory = new Byte[size];
}


Memory::~Memory()
{
    delete[] memory;
}


Byte Memory::getByte(Address address) const
{
    return memory[address];
}


long Memory::getLong(Address address) const
{
    return ((long*) memory)[address];
}


bool Memory::getBit(Address address) const
{
    return ((bool*) memory)[address];
}


void Memory::set(Address address, Byte value)
{
    memory[address] = value;
}


void Memory::set(Address address, long value)
{
    ((long*) memory)[address] = value;
}


void Memory::set(Address address, bool value)
{
    ((bool*) memory)[address] = value;
}

