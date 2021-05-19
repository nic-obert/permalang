#include "pvm.hh"


using namespace pvm;


ByteNode::ByteNode(Value data, InstructionSize dataSize)
: data(data), dataSize(dataSize)
{

}


ByteNode::ByteNode(OpCode data)
: data(toValue(data)), dataSize(1)
{
    
}

