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


void ByteList::extend(ByteList& other)
{
    if (other.start != nullptr)
    {
        other.start->prev = end;
    }

    if (end != nullptr)
    {
        end->next = other.start;
    }

    if (other.end != nullptr)
    {
        end = other.end;
    }

    nodeCount += other.nodeCount;
    byteSize += other.byteSize;

    other.clear();
}


void ByteList::clear()
{
    start = nullptr;
    end = nullptr;
    byteSize = 0;
    nodeCount = 0;
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


size_t ByteList::getCurrentSize() const
{
    return byteSize;
}

