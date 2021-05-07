#include "tac.hh"


// treats the Byte[] array as a long[] array
#define longArray *((long*) (bytes + index))

// the left capacity in bytes that triggers Byte array resize
// this is the size of the largest TAC instruction translated to ByteCode
#define TRIGGER_CAPACITY 30

// the amount to increase the Byte array by when resizing
#define RESIZE_AMOUNT size


using namespace tac;


void compileBinaryOperation(pvm::OpCode operation, const TacInstruction* instruction, pvm::Byte* bytes, size_t& index)
{
    using namespace pvm;

// load values inside registers

// first operand

    if (instruction->addr2.type == TacValueType::ADDRESS)
    {
        // load from memory instruction
        bytes[index] = (Byte) OpCode::LDA;
    }
    else // if (instruction->addr2.type == TacValueType::LITERAL)
    {
        // load constant literal
        bytes[index] = (Byte) OpCode::LDCA;
    }
    index ++;

    // the first operand
    longArray = instruction->addr2.value;
    index += sizeof(long);

// second operand

    if (instruction->addr3.type == TacValueType::ADDRESS)
    {
        // load from memory instruction
        bytes[index] = (Byte) OpCode::LDB;
    }
    else // if (instruction->addr3.type == TacValueType::LITERAL)
    {
        // load constant literal
        bytes[index] = (Byte) OpCode::LDCB;
    }
    index ++;

    // the first operand
    longArray = instruction->addr3.value;
    index += sizeof(long);


// perform the actual operation

    bytes[index] = (Byte) operation;
    index ++;

// copy the result into the destination address

    // copy value from register to memory address
    bytes[index] = (Byte) OpCode::REG_MOV;
    index ++;

    // the memory address to copy the result to
    longArray = instruction->addr1.value;
    index += sizeof(long);

    // the register where to copy the result from
    bytes[index] = (Byte) Registers::RGA;
    index ++;

}


pvm::Byte* Tac::toByteCode() const
{
    using namespace pvm;

    std::unordered_map<void*, size_t> labels = std::unordered_map<void*, size_t>();

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
        
        
        // instruction size: 1 byte
        case TacOp::LABEL:
        {   
            // save index of the label
            labels[instruction] = index;
            // increment index for next instruction
            index ++;
            break;
        }

        
        // instruction size: 9 bytes
        case TacOp::JUMP:
        {
            // add unconditional jump instruction
            bytes[index] = (Byte) OpCode::JMP;
            index ++;

            // add label to jump to
            longArray = instruction->addr1.value; 
            index += sizeof(Value);
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
            longArray = instruction->addr1.value;

            // the value the variable is being set to
            longArray = instruction->addr2.value;
            index += sizeof(long);

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
                bytes[index] = (Byte) OpCode::LDA;
            }
            else // if (instruction->addr2.type == TacValueType::LITERAL)
            {
                // load constant literal
                bytes[index] = (Byte) OpCode::LDCA;
            }
            index ++;

            // the first operand
            longArray = instruction->addr2.value;
            index += sizeof(long);

        // second operand

            if (instruction->addr3.type == TacValueType::ADDRESS)
            {
                // load from memory instruction
                bytes[index] = (Byte) OpCode::LDB;
            }
            else // if (instruction->addr3.type == TacValueType::LITERAL)
            {
                // load constant literal
                bytes[index] = (Byte) OpCode::LDCB;
            }
            index ++;

            // the first operand
            longArray = instruction->addr3.value;
            index += sizeof(long);


        // perform the actual operation

            bytes[index] = (Byte) OpCode::CMP;
            index ++;

        // copy the result into the destination address

            // copy value from register to memory address
            bytes[index] = (Byte) OpCode::REG_MOV_BIT;
            index ++;

            // the memory address to copy the result to
            longArray = instruction->addr1.value;
            index += sizeof(long);

            // the register where to copy the result from (zero flag)
            bytes[index] = (Byte) Registers::RZF;
            index ++;

            break;
        }


        case TacOp::IF:
        {
            // load condition
            bytes[index] = (Byte) OpCode::LDA;
            index ++;

            // condition's address
            longArray = instruction->addr1.value;
            index += sizeof(long);

            // load const 0 to be compared with the condition (invert condition)
            bytes[index] = (Byte) OpCode::LDCB;
            index ++;
            bytes[index] = 0;
            index ++;

            // evaluate condition
            bytes[index] = (Byte) OpCode::CMP;
            index ++;

            // actual if instruction
            bytes[index] = (Byte) OpCode::IF_JUMP;
            index ++;

            // add label to jump to
            longArray = instruction->addr2.value; 
            index += sizeof(Value); 

            break;
        }

        
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

        
        } // switch (instruction->operation)


    } // for (instruction in instructions)


    // TODO fill in labels 

    // allocate a new ByteCode with the size of index + 1
    // to leave place for the final EXIT instruction
    Byte* bytecode = new Byte[index + 1];

    // copy the byte array into a ByteCode to be returned
    // TODO fill in labels here with a for loop instead of memcpying 
    memcpy(bytecode, bytes, index - 1);

    // set the last Byte of ByteCode to the EXIT OpCode
    bytecode[index] = (Byte) OpCode::EXIT;
    index ++;
    // exit code should be 0 if program terminated successfully
    bytecode[index] = 0;

    // finally delete bytes array
    delete[] bytes;

    return bytecode;
}

