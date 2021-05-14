#include "tac.hh"


using namespace tac;


Tac::Tac()
: start(nullptr), end(nullptr), blockCount(0)
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

    blockCount ++;
}


void Tac::extend(Tac& _tac)
{   
    // check for uninitialized values
    if (start == nullptr)
    {
        start = _tac.start;
    }
    else // if (start != nullptr)
    {
        // just steal the other TAC's CodeBlocks
        end->setNext(_tac.start);
    }

    _tac.start->setPrev(end);
    end = _tac.end;

    _tac.start = nullptr;
    _tac.end = nullptr;

    blockCount += _tac.blockCount;
}


const CodeBlock* Tac::getStart() const
{
    return start;
}


const CodeBlock* Tac::getEnd() const
{
    return end;
}


std::ostream& operator<<(std::ostream& stream, const tac::Tac& _tac)
{
    stream << "Three Address Code: {\n";

    for (const CodeBlock* block = _tac.getStart(); block != nullptr; block = block->getNext())
    {
        stream << *block << "\n" << "---\n\n";
    }

    return stream << "}";
}


void Tac::declareSymbols(const symbol_table::Scope* scope)
{
    end->initSymbols(scope);
}


void Tac::popSymbols(const symbol_table::Scope* scope)
{
    end->popSymbols(scope);
}

