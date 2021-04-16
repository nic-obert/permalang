#pragma once

#include "token.hh"
#include "syntax_tree.hh"
#include "op_codes.hh"


namespace operators{};


namespace operators::arithmetical
{

    const char* arithmeticalOperatorName(OpCodes op);

};


namespace operators::logical
{

    const char* logicalOperatorName(OpCodes op);
    
};


namespace operators::assignment
{

    const char* assignmentOperatorName(OpCodes op);

};


namespace operators::scope
{

    

};
