#pragma once

#include "pch.hh"

#include "syntax_tree.hh"
#include "pvm.hh"


namespace tac
{   

    // basic operations from which to generate TAC
    typedef enum class TacOp
    {
        LABEL,
        IF,
        JUMP,
        ASSIGN,
        SUM,
        SUB,
        MUL,
        DIV,
        EQ,
        LESS,
        NO_OP,

    } TacOp;


    // the type of a TacValue, either:
    //  - LITERAL = 0
    //  - ADDRESS = 1
    //  - LABEL = 2
    typedef enum class TacValueType
    {
        LITERAL = 0, // 0 --> false
        ADDRESS = 1, // 1 --> true
        LABEL = 2    // 2

    } TacValueType;

    
    // a class representing both temporary and real memory addresses
    class Address
    {
    private:

        // Addresses can only be obtained via getAddress()
        Address();

        // map that keeps track of temporary memory addresses
        static std::unordered_map<Value, unsigned int> addresses;

    public:

        // returns whether the given memory address is a temporary address
        // aka instantiated with getAddress()
        static bool isTempAddress(const Value address);

        // TODO implement a more efficient way of getting addresses
        static const Address* getAddress();

        // TODO free addresses based on scope to avoid memory leaks
        static void free(const Address* const address);

    };


    // holds a Value and a TacValueType
    typedef struct TacValue 
    {
        TacValueType type;
        Value value;

        // unused constructor
        TacValue();
        // actually used constructor
        TacValue(TacValueType type, Value value);

    } TacValue;


    class TacInstruction
    {
    public:

        // next element in doubly-linked list
        TacInstruction* next;
        // previous element in doubly-linked list
        // not initialized since it will be by the Tac::add() function
        TacInstruction* prev;

        // the basic operation performed by the instruction
        TacOp operation;

        // the 3 addresses of three address code

        TacValue addr1;
        TacValue addr2;
        TacValue addr3;

        // constructors differenciated by number of operands (0,1,2,3)

        TacInstruction(TacOp operation);
        TacInstruction(TacOp operation, TacValue&& addr1);
        TacInstruction(TacOp operation, TacValue&& addr1, TacValue&& addr2);
        TacInstruction(TacOp operation, TacValue&& addr1, TacValue&& addr2, TacValue&& addr3);

    };


    class CodeBlock
    {
    private:

        // start of the doubly-linked list
        TacInstruction* start;
        // end of the doubly-linked list
        TacInstruction* end;

        // TODO implement symbol declaration list

        // instruction count
        size_t size;

        // previous element in doubly-linked list
        CodeBlock* prev;
        // next element in doubly-linked list
        CodeBlock* next;

    public:

        CodeBlock(CodeBlock* prev);
        CodeBlock() = delete;

        // add an instrution to the block
        // increment the block's size
        // performs nullptr checks
        void add(TacInstruction* instruction);

        
        const TacInstruction* getStart() const;
        const TacInstruction* getEnd() const;

        const CodeBlock* getPrev() const;
        const CodeBlock* getNext() const;

        void setNext(CodeBlock* block);
        void setPrev(CodeBlock* block);

    };


    // Three Address Code
    class Tac
    {
    private:

        // start of doubly-linked list of CodeBlock
        CodeBlock* start;
        // end of doubly-linked list of CodeBlock
        CodeBlock* end;


        // recursive function that generates TAC for a complex operation
        // returns a reference to the result of the operation
        const Address* tacFor(Tokens::Token* token, Tokens::Token** operands);
        const Address* tacFor(OpCodes opCode, Tokens::Token** operands);

    public:

        Tac();

        // extend the TAC with a new codeBlock
        // performs nullptr checks
        void extend(CodeBlock* codeBlock);
        // extend the TAC with another TAC
        // performs nullptr checks
        // possibly destructive
        void extend(Tac& tac);

        // recursive function that parses recursively an operator Token
        // sets the token's value to an Address* which stores
        // the result of the operation
        void parseOperator(Tokens::Token* token);

        // returns the ByteCode for the generated TAC instructions
        pvm::Byte* toByteCode() const; 

        const CodeBlock* getStart() const;
        const CodeBlock* getEnd() const;

    };

};


std::ostream& operator<<(std::ostream& stream, const tac::Tac& tac);

std::ostream& operator<<(std::ostream& stream, const tac::TacInstruction& instruction);

std::ostream& operator<<(std::ostream& stream, const tac::TacValue& value);

std::ostream& operator<<(std::ostream& stream, const tac::TacOp& op);

std::ostream& operator<<(std::ostream& stream, const tac::Address* const address);

std::ostream& operator<<(std::ostream& stream, const tac::CodeBlock& codeBlock);

