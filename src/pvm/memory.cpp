#include "pvm.hh"


using namespace pvm;


Memory::Memory(size_t size)
{
    memory = new Byte[size];
}


Memory::~Memory()
{
    delete memory;
}


Byte Memory::getByte(Address address) const
{
    return memory[address];
}


long Memory::getLong(Address address) const
{
    return ((long*) memory)[address];
}


void Memory::set(Address address, Byte value)
{
    memory[address] = value;
}


void Memory::set(Address address, long value)
{
    ((long*) memory)[address] = value;
}

