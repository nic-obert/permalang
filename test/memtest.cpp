#include "pvm.hh"

using namespace pvm;
using namespace std;


int main()
{
    Memory mem = Memory(64);

    mem.set(0, (Byte)34);
    cout << (int) mem.getByte(0) << endl;
    mem.set(1, (Byte)3);

    mem.set(16, (long) 1069);
    cout << mem.getLong(16) << endl;

    cout << mem.getLong(0) << endl;



}