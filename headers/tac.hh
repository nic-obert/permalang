#pragma once


#include "utils.hh"
#include "syntax_tree.hh"
#include "token.hh"

#include <iostream>


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

    public:

        // TODO implement a more efficient way of getting addresses
        static const Address* getAddress();

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

        TacInstruction* next;
        TacInstruction* prev;

        TacOp operation;

        TacValue* addr1;
        TacValue* addr2;
        TacValue* addr3;


        TacInstruction(TacOp operation);

    };


    class Tac
    {
    private:

        // linked list of instructions
        TacInstruction* instructions;

        void add(TacInstruction* in);

        // recursive function that parses a single operator token
        void parseOperator(const Tokens::Token* token);

    public:

        TacInstruction* start;

        Tac();

        // transforms a syntax tree to a tac instructions linked list
        void parseTree(syntax_tree::SyntaxTree const& tree);

    };

};


std::ostream& operator<<(std::ostream& stream, tac::Tac const& tac);

std::ostream& operator<<(std::ostream& stream, tac::TacInstruction const& instruction);

std::ostream& operator<<(std::ostream& stream, tac::TacValue const& value);

std::ostream& operator<<(std::ostream& stream, tac::TacOp const& op);

std::ostream& operator<<(std::ostream& stream, tac::Address const& address);


