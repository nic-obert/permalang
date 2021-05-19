#include "tac.hh"


// the left capacity in bytes that triggers Byte array resize
// this is the size of the largest TAC instruction translated to ByteCode
#define TRIGGER_CAPACITY 30

// the amount to increase the Byte array by when resizing
#define RESIZE_AMOUNT size

// maps a label (TacInstruction*) to an instruction index
// stores the instruction index to jump to, corresponding to the label
typedef std::unordered_map<Value, size_t> LabelTable;


using namespace tac;


// utility function
// sets the byte array at the given index to the given value
// updates index
static inline void setLongValue(pvm::Byte* bytes, size_t& index, long value)
{
    *((long*) (bytes + index)) = value;
    index += sizeof(long);
}


static void compileBinaryOperation(pvm::OpCode operation, const TacInstruction* instruction, pvm::Byte* bytes, size_t& index)
{
    using namespace pvm;

// load values inside registers

// first operand

    if (instruction->addr2.type == TacValueType::ADDRESS)
    {
        // load from memory instruction
        bytes[index] = (Byte) OpCode::LD_A;
    }
    else // if (instruction->addr2.type == TacValueType::LITERAL)
    {
        // load constant literal
        bytes[index] = (Byte) OpCode::LD_CONST_A;
    }
    index ++;

    // the first operand
    setLongValue(bytes, index, instruction->addr2.value);

// second operand

    if (instruction->addr3.type == TacValueType::ADDRESS)
    {
        // load from memory instruction
        bytes[index] = (Byte) OpCode::LD_B;
    }
    else // if (instruction->addr3.type == TacValueType::LITERAL)
    {
        // load constant literal
        bytes[index] = (Byte) OpCode::LD_CONST_B;
    }
    index ++;

    // the first operand
    setLongValue(bytes, index, instruction->addr3.value);


// perform the actual operation

    bytes[index] = (Byte) operation;
    index ++;

// copy the result into the destination address

    // copy value from register to memory address
    bytes[index] = (Byte) OpCode::REG_MOV;
    index ++;

    // the memory address to copy the result to
    setLongValue(bytes, index, instruction->addr1.value);

    // the register where to copy the result from
    bytes[index] = (Byte) Registers::RESULT;
    index ++;

}


// fills the labels' placeholders in bytecode, given a label table
static void fillLabels(pvm::Byte* byteCode, const LabelTable& labels, std::vector<size_t>& jumpIndexes)
{
    for (size_t index : jumpIndexes)
    {
        // get the label, treating the byteCode as a long array
        Value label = *((long*) (byteCode + index + 1));
        // get the corresponding index to jump to
        size_t target = labels.at(label);
        // replace the old label with the new target
        *((long*) (byteCode + index + 1)) = target;
    }
}


pvm::ByteCode Tac::toByteCode() const
{
    using namespace pvm;

    // create the final code block that will hold all code blocks
    CodeBlock finalBlock = CodeBlock();

    // extend the final CodeBlock with all CodeBlocks
    for (const CodeBlock* block = start; block != nullptr; block = block->getNext())
    {
        finalBlock.extend(block);
    }

    // compile the final CodeBlock to byte code
    // reserve 2 more bytes to add the final OpCode::EXIT instruction
    ByteCode byteCode = finalBlock.toByteCode(2);

    // add the final OpCode::EXIT instruction
    byteCode.byteCode[byteCode.size - 2] = (Byte) OpCode::EXIT;
    byteCode.byteCode[byteCode.size - 1] = 0;

    return byteCode;
}


pvm::ByteCode CodeBlock::toByteCode(size_t reserveBytes) const
{
    using namespace pvm;


    LabelTable labels = LabelTable();

    // vector of the indexes of jump instructions
    std::vector<size_t> jumpIndexes = std::vector<size_t>();
    jumpIndexes.reserve(size);



    // reserve at least the size of TAC instructions * 3
    // most operators require 2 operands (min 3 bytes per instruction)
    size_t capacity = size * TRIGGER_CAPACITY;
    Byte* bytes = new Byte[capacity];

    size_t index = 0;

    for (TacInstruction* instruction = start; instruction != nullptr; instruction = instruction->next)
    {
        switch (instruction->operation)
        {

        // instruction size: 0 bytes
        case TacOp::NO_OP:
            // do not increment index sinze NO_OP won't be added to the ByteCode
            continue;
        
        
        // instruction size: 0 bytes
        case TacOp::LABEL:
        {   
            // save index of the declared label
            labels[toValue(instruction)] = index;
            // do not increment index as labels do not occupy physical space 
            break;
        }

        
        // instruction size: 9 bytes
        case TacOp::JUMP:
        {
            // add the jump instruction index to the jumpIndexes vector
            jumpIndexes.push_back(index);

            bytes[index] = (Byte) OpCode::JMP;
            index ++;

            // add label to jump to
            setLongValue(bytes, index, instruction->addr1.value);
            break;
        }

        
        // instruction size: 17 bytes
        case TacOp::ASSIGN:
        {
            if (instruction->addr2.type == TacValueType::ADDRESS)
            {
                bytes[index] = (Byte) OpCode::MEM_SET;
                index ++;
            } 
            else // if (instruction->addr2.type == TacValueType::LITERAL)
            {
                bytes[index] = (Byte) OpCode::MEM_MOV;
                index ++;
            }

            // get the variable the value is being assigned to
            setLongValue(bytes, index, instruction->addr1.value);

            // the value the variable is being set to
            setLongValue(bytes, index, instruction->addr2.value);

            break;
        }
        
        
        // instruction size: 29 bytes
        case TacOp::SUM:
        {   
            compileBinaryOperation(OpCode::ADD, instruction, bytes, index);
            break;
        }
        

        case TacOp::SUB:
        {
            compileBinaryOperation(OpCode::SUB, instruction, bytes, index);
            break;
        }
        

        case TacOp::MUL:
        {
            compileBinaryOperation(OpCode::MUL, instruction, bytes, index);
            break;
        }


        case TacOp::DIV:
        {
            compileBinaryOperation(OpCode::DIV, instruction, bytes, index);
            break;
        }

        
        case TacOp::EQ:
        {
            
            // load values inside registers

        // first operand

            if (instruction->addr2.type == TacValueType::ADDRESS)
            {
                // load from memory instruction
                bytes[index] = (Byte) OpCode::LD_A;
            }
            else // if (instruction->addr2.type == TacValueType::LITERAL)
            {
                // load constant literal
                bytes[index] = (Byte) OpCode::LD_CONST_A;
            }
            index ++;

            // the first operand
            setLongValue(bytes, index, instruction->addr2.value);

        // second operand

            if (instruction->addr3.type == TacValueType::ADDRESS)
            {
                // load from memory instruction
                bytes[index] = (Byte) OpCode::LD_B;
            }
            else // if (instruction->addr3.type == TacValueType::LITERAL)
            {
                // load constant literal
                bytes[index] = (Byte) OpCode::LD_CONST_B;
            }
            index ++;

            // the first operand
            setLongValue(bytes, index, instruction->addr3.value);


        // perform the actual operation

            bytes[index] = (Byte) OpCode::CMP;
            index ++;

        // copy the result into the destination address

            // copy value from register to memory address
            bytes[index] = (Byte) OpCode::REG_MOV_BIT;
            index ++;

            // the memory address to copy the result to
            setLongValue(bytes, index, instruction->addr1.value);

            // the register where to copy the result from (zero flag)
            bytes[index] = (Byte) Registers::ZERO_FLAG;
            index ++;

            break;
        }


        case TacOp::IF:
        {
            // load condition
            bytes[index] = (Byte) OpCode::LD_A;
            index ++;

            // condition's address
            setLongValue(bytes, index, instruction->addr1.value);

            // load const 0 to be compared with the condition (invert condition)
            bytes[index] = (Byte) OpCode::LD_CONST_B;
            index ++;
            bytes[index] = 0;
            index ++;

            // evaluate condition
            bytes[index] = (Byte) OpCode::CMP;
            index ++;

            // add the conditional jump instruction's index to the jumpIndexes
            jumpIndexes.push_back(index);

            // actual if instruction
            bytes[index] = (Byte) OpCode::IF_JUMP;
            index ++;

            // add label to jump to
            setLongValue(bytes, index, instruction->addr2.value); 

            break;
        }

        
        case TacOp::PUSH:
        { 
            bytes[index] = (Byte) OpCode::PUSH_BYTES;
            index ++;

            // bytes to push (the size of the scope)
            setLongValue(bytes, index, instruction->addr1.value);

            break;
        }

        
        case TacOp::POP:
        {
            // pop instruction
            bytes[index] = (Byte) OpCode::POP;
            index ++;

            // bytes to pop
            setLongValue(bytes, index, instruction->addr1.value);

            break;
        }

        
        } // switch (instruction->operation)


    // END OF case TacOp::*:


        // check if bytes' capacity is near limit
        // if so, increase its capacity
        if (capacity - index <= TRIGGER_CAPACITY)
        {
            // save old size and increase capacity by TAC size (arbitrary)
            size_t oldSize = capacity;
            capacity += RESIZE_AMOUNT;

            // create a new Byte array and copy the old array into it
            Byte* newArray = new Byte[capacity];
            memcpy(newArray, bytes, oldSize);

            // delete the old array and set bytes to point to the new one
            delete[] bytes;
            bytes = newArray;
        }


    } // for (instruction in instructions)


    // create a ByteCode object to hold the byte code
    // allocate a new ByteCode with the size of index - 1
    ByteCode byteCode = ByteCode(new Byte[index + reserveBytes], index + reserveBytes);

    // copy the byte array into a ByteCode to be returned
    memcpy(byteCode.byteCode, bytes, byteCode.size);

    // fill in labels based on the label table 
    fillLabels(byteCode.byteCode, labels, jumpIndexes);

    // finally delete bytes array
    delete[] bytes;

    return byteCode;
}

