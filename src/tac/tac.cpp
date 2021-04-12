#include "tac.hh"
#include "token.hh"
#include "syntax_tree.hh"
#include "op_codes.hh"
#include "utils.hh"


using namespace tac;


Tac::Tac()
{
    // initialize with the zeroth instruction
    start = new TacInstruction(TacOp::NO_OP);
    instructions = start;
}


void Tac::add(TacInstruction* in)
{
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

