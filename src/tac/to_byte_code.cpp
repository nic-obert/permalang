#include "tac.hh"


// treats the Byte[] array as a long[] array
#define longArray *((long*) (bytes + index))

// the left capacity in bytes that triggers Byte array resize
// this is the size of the largest TAC instruction translated to ByteCode
#define TRIGGER_CAPACITY 27

// the amount to increase the Byte array by when resizing
#define RESIZE_AMOUNT size


using namespace tac;


pvm::ByteCode Tac::toByteCode() const
{
    using namespace pvm;

    std::unordered_map<void*, size_t> labels = std::unordered_map<void*, size_t>();

    // reserve at least the size of TAC instructions * 3
    // most operators require 2 operands (min 3 bytes per instruction)
    size_t capacity = size * 3;
    Byte* bytes = new Byte[capacity];

    size_t index;

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

        
        // instruction size: 1 + 8 = 9 bytes
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

        
        // instruction size: 1 + 8 + 8 = 17 bytes
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
        
        
        // instruction size: 1 + 8 + 1 + 8 + 1 + 1 + 8 + 1 = 29 bytes
        case TacOp::SUM:
        {   
            // load values inside registers

        // first operand

            if (instruction->addr2.type == TacValueType::ADDRESS)
            {
                // load from memory instruction
                bytes[index] = (Byte) OpCode::MEM_LD;
            }
            else // if (instruction->addr2.type == TacValueType::LITERAL)
            {
                // load constant literal
                bytes[index] = (Byte) OpCode::LD;
            }
            index ++;

            // the register to load the value in
            bytes[index] = GP_A;
            index ++;

            // the first operand
            longArray = instruction->addr2.value;
            index += sizeof(long);

        // second operand

            if (instruction->addr3.type == TacValueType::ADDRESS)
            {
                // load from memory instruction
                bytes[index] = (Byte) OpCode::MEM_LD;
            }
            else // if (instruction->addr3.type == TacValueType::LITERAL)
            {
                // load constant literal
                bytes[index] = (Byte) OpCode::LD;
            }
            index ++;

            // the register to load the value in
            bytes[index] = GP_B;
            index ++;

            // the first operand
            longArray = instruction->addr3.value;
            index += sizeof(long);


        // perform the actual sum operation

            bytes[index] = (Byte) OpCode::ADD;
            index ++;

        // copy the result into the destination address

            // copy value from register to memory address
            bytes[index] = (Byte) OpCode::REG_MOV;
            index ++;

            // the memory address to copy the result to
            longArray = instruction->addr1.value;
            index += sizeof(long);

            // the register where to copy the result from
            bytes[index] = GP_A;
            index ++;

            break;
        }
        

        // check if bytes' capacity is near limit
        // if so, increase its capacity
        if (capacity <= TRIGGER_CAPACITY)
        {
            // save old size and increase capacity by TAC size (arbitrary)
            size_t oldSize = capacity;
            capacity += RESIZE_AMOUNT;

            // create a new Byte array and copy the old array into it
            Byte* newArray = new Byte[capacity];
            memcpy(newArray, bytes, oldSize);

            // delete the old array and set bytes to point to the new one
            delete bytes;
            bytes = newArray;            
        }

        
        } // switch (instruction->operation)


    } // for (instruction in instructions)


    // TODO transform bytes into ByteCode, remove redundant bytes and fill in labels 

    bytes[index] = (Byte) OpCode::EXIT;

    // finally delete bytes array
    delete bytes;

}

