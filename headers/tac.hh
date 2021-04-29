#pragma once

#include "pch.hh"

#include "utils.hh"
#include "syntax_tree.hh"
#include "token.hh"


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
        NO_OP
    } TacOp;


    // the type of a TacValue, either LITERAL, ADDRESS or LABEL
    typedef enum class TacValueType
    {
        LITERAL, // 0 --> false
        ADDRESS, // 1 --> true
        LABEL    // 2
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
        TacInstruction* next = nullptr;
        // previous element in doubly-linked list
        TacInstruction* prev = nullptr;

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


    // Three Address Code
    class Tac
    {
    private:

        // last element of doubly linked list of TacInstructions
        TacInstruction* instructions;

        // add an instruction to the TacInstruction doubly-linked list
        void add(TacInstruction* in);

        // recursive function that parses recursively an operator Token
        // sets the token's value to an Address* which stores
        // the result of the operation
        void parseOperator(Tokens::Token* token);

        // recursive function that generates TAC for a complex operation
        // returns a reference to the result of the operation
        const Address* tacFor(Tokens::Token* token, Tokens::Token** operands);
        const Address* tacFor(OpCodes opCode, Tokens::Token** operands);

    public:

        // boolean false
        static const bool NO_LABEL;

        // first element of the TacInstruction doubly linked list
        TacInstruction* start = nullptr;

        Tac();

        // transforms a syntax tree to a tac instructions linked list
        // returns a label to the TAC generated for the given SyntaxTree
        TacInstruction* parseTree(syntax_tree::SyntaxTree& tree, bool addLabel = true);

    };

};


std::ostream& operator<<(std::ostream& stream, tac::Tac const& tac);

std::ostream& operator<<(std::ostream& stream, tac::TacInstruction const& instruction);

std::ostream& operator<<(std::ostream& stream, tac::TacValue const& value);

std::ostream& operator<<(std::ostream& stream, tac::TacOp const& op);

std::ostream& operator<<(std::ostream& stream, const tac::Address* const address);


