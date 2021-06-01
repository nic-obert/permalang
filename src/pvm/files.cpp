#include "pvm.hh"
#include "errors.hh"

#include "pch.hh"


using namespace pvm;


void pvm::generateExecutable(ByteCode byteCode, const char* name)
{
    std::ofstream file(name, std::ios::binary);

    if (!file.is_open())
    {
        errors::FileWriteError(name);
    }

    // first 8 bytes are the executable size
    file.write((const char*) &byteCode.size, sizeof(size_t));

    file.write((const char*) byteCode.byteCode, byteCode.size);

    file.close();
}


ByteCode pvm::loadByteCode(const char* executable)
{
    std::ifstream file(executable, std::ios::binary);

    if (!file.is_open())
    {
        errors::FileReadError(executable);
    }

    // read the first 8 bytes of the file to get the size
    size_t size;
    file.read((char*) &size, sizeof(size_t));

    Byte* bytes = new Byte[size];
    file.read((char*) bytes, size);

    return ByteCode(bytes, size);
}

