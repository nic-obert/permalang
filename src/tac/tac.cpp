#include "tac.hh"


using namespace tac;


const bool Tac::NO_LABEL = false;


Tac::Tac()
: size(1)
{
    /*
        initialize with the zeroth instruction so not 
        to have to check for null pointers when adding
        a new instruction
    */
    start = new TacInstruction(TacOp::NO_OP);
    instructions = start;
    start->prev = nullptr;
}


void Tac::add(TacInstruction* in)
{
    /*
        just add the instruction without checking for
        null pointers because the first element of the list
        is always initialized in the constructor
    */
    instructions->next = in;
    in->prev = instructions;
    instructions = in;
    // increment size
    size ++;
}


std::ostream& operator<<(std::ostream& stream, tac::Tac const& tac)
{
    stream << "Three Address Code: {\n";

    for (const TacInstruction* instruction = tac.start; instruction != nullptr; instruction = instruction->next)
    {
        // do not add a tab if instruction is a label for readability
        if (instruction->operation != TacOp::LABEL)
        {
            stream << '\t';
        }
        stream << *instruction << "\n";
    }

    return stream << "}";
}

