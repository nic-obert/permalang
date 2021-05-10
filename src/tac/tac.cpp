#include "tac.hh"


using namespace tac;


Tac::Tac()
: start(nullptr), end(nullptr)
{

}


void Tac::extend(CodeBlock* codeBlock)
{
    if (start == nullptr)
    {
        start = codeBlock;
    }
    else // if (start != nullptr)
    {
        end->setNext(codeBlock);
    }

    codeBlock->setPrev(end);
    end = codeBlock;
}


void Tac::extend(Tac& tac)
{
    // just steal the other TAC's CodeBlocks
    end->setNext(tac.start);
    end = tac.end;
}


const CodeBlock* Tac::getStart() const
{
    return start;
}


const CodeBlock* Tac::getEnd() const
{
    return end;
}


std::ostream& operator<<(std::ostream& stream, const tac::Tac& tac)
{
    stream << "Three Address Code: {\n";

    for (const CodeBlock* block = tac.getStart(); block != nullptr; block = block->getNext())
    {
        stream << *block << "\n" << "---\n\n";
    }

    return stream << "}";
}

