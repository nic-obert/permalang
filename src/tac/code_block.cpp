#include "tac.hh"


using namespace tac;


CodeBlock::CodeBlock(CodeBlock* prev)
: prev(prev), next(nullptr), start(nullptr), end(nullptr)
{

}


void CodeBlock::add(TacInstruction* instruction)
{
    if (start == nullptr)
    {
        start = instruction;
    }
    else // if (start != nullptr)
    {
        end->next = instruction;
    }

    // insert instruction in list
    instruction->prev = end;
    end = instruction;

    // increase block size
    size ++;
}


const TacInstruction* CodeBlock::getStart() const
{
    return start;
}


const TacInstruction* CodeBlock::getEnd() const
{
    return end;
}


const CodeBlock* CodeBlock::getNext() const
{
    return next;
}


const CodeBlock* CodeBlock::getPrev() const
{
    return prev;
}


void CodeBlock::setNext(CodeBlock* block)
{
    next = block;
}


void CodeBlock::setPrev(CodeBlock* block)
{
    prev = block;
}


std::ostream& operator<<(std::ostream& stream, const tac::CodeBlock& codeBlock)
{
    for (const TacInstruction* instruction = codeBlock.getStart(); instruction != nullptr; instruction = instruction->next)
    {
        // do not add a tab if instruction is a label for readability
        if (instruction->operation != TacOp::LABEL)
        {
            stream << '\t';
        }
        stream << *instruction << "\n";
    }

    return stream;
}

