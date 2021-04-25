#include "tac.hh"
#include "token.hh"
#include "syntax_tree.hh"
#include "op_codes.hh"
#include "utils.hh"


using namespace tac;


const bool Tac::NO_LABEL = false;


Tac::Tac()
{
    /*
        initialize with the zeroth instruction so not 
        to have to check for null pointers when adding
        a new instruction
    */
    start = new TacInstruction(TacOp::NO_OP);
    instructions = start;
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
}


std::ostream& operator<<(std::ostream& stream, tac::Tac const& tac)
{
    stream << "Three Address Code: {\n";

    for (const TacInstruction* instruction = tac.start; instruction != nullptr; instruction = instruction->next)
    {
        stream << "\t" << *instruction << "\n";
    }

    return stream << "}";
}


std::ostream& operator<<(std::ostream& stream, tac::Tac const* tac)
{
    return stream << *tac;
}

