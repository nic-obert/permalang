#include "syntax_tree.hh"


using namespace syntax_tree;


ControlFlowNode::ControlFlowNode(pvm::ByteNode* byteNode, OpCodes opCode)
: byteNode(byteNode), opCode(opCode)
{

}

