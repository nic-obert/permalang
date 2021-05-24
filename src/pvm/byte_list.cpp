#include "pvm.hh"


using namespace pvm;


ByteList::ByteList()
: start(nullptr), end(nullptr), nodeCount(0), byteSize(0)
{

}


ByteList::~ByteList()
{
    for (ByteNode* node = start; node != nullptr; )
    {
        ByteNode* tmp = node;
        node = node->next;
        delete tmp;
    }
}


void ByteList::add(ByteNode* node)
{
    node->next = nullptr;
    node->prev = end;

    if (start == nullptr)
    {
        start = node;
    }
    else
    {
        end->next = node;
    }

    end = node;

    nodeCount ++;
    byteSize += node->dataSize;
}


void ByteList::insertFisrt(ByteNode* node)
{
    node->next = start;
    node->prev = nullptr;

    if (start != nullptr)
    {
        start->prev = node;
    }
    else
    {
        end = node;
    }

    start = node;

    nodeCount ++;
    byteSize += node->dataSize;
}


ByteCode ByteList::toByteCode() const
{

    Byte* bytes = new Byte[byteSize];

    ByteCode byteCode = ByteCode(bytes, byteSize);

    size_t i = 0;
    for (ByteNode* node = start; node != nullptr; node = node->next)
    {
        // copy "dataSize" bytes to the array of bytes from the node's data
        memcpy(bytes + i, &node->data, node->dataSize);
        i += node->dataSize;
    }

    return byteCode;
}

