#pragma once


#include "utils.hh"
#include "syntax_tree.hh"
#include "token.hh"

#include <iostream>
#include <unordered_map>


#define BASE_ADDRESSES 8


namespace tac
{   

    typedef enum TacOp
    {
        IF,
        JUMP,
        LABEL,
        ASSIGN,
        SUM,
        MUL,
        DIV,
        SUB,
        NO_OP
    } TacOp;


    class Address
    {
    private:

        // Addresses can only be obtained via getAddress()
        Address();

        static std::unordered_map<Value, unsigned int> addresses;

    public:

        static bool isTempAddress(Value address);

        // TODO implement a more efficient way of getting addresses
        static const Address* getAddress();

        // TODO free addresses based on scope to avoid memory leaks
        static void free(const Address* address);

    };


    typedef struct TacValue 
    {
        bool isAddress;
        Value value;

        TacValue(bool isAddress, Value value);

    } TacValue;


    class TacInstruction
    {
    public:

        TacInstruction* next = nullptr;
        TacInstruction* prev = nullptr;

        TacOp operation;

        TacValue* addr1 = nullptr;
        TacValue* addr2 = nullptr;
        TacValue* addr3 = nullptr;


        TacInstruction(TacOp operation);
        TacInstruction(TacOp operation, TacValue* addr1);
        TacInstruction(TacOp operation, TacValue* addr1, TacValue* addr2);
        TacInstruction(TacOp operation, TacValue* addr1, TacValue* addr2, TacValue* addr3);

    };


    class Tac
    {
    private:

        // linked list of instructions
        TacInstruction* instructions;

        void add(TacInstruction* in);

        // recursive function that parses a single operator token
        void parseOperator(Tokens::Token* token);

    public:

        TacInstruction* start = nullptr;

        Tac();

        // transforms a syntax tree to a tac instructions linked list
        void parseTree(syntax_tree::SyntaxTree& tree);

    };

};


std::ostream& operator<<(std::ostream& stream, tac::Tac const& tac);
std::ostream& operator<<(std::ostream& stream, tac::Tac const* tac);

std::ostream& operator<<(std::ostream& stream, tac::TacInstruction const& instruction);
std::ostream& operator<<(std::ostream& stream, tac::TacInstruction const* instruction);

std::ostream& operator<<(std::ostream& stream, tac::TacValue const& value);
std::ostream& operator<<(std::ostream& stream, tac::TacValue const* value);

std::ostream& operator<<(std::ostream& stream, tac::TacOp const& op);

std::ostream& operator<<(std::ostream& stream, tac::Address const* address);


