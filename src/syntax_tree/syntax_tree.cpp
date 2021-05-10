#include "syntax_tree.hh"


using namespace syntax_tree;



SyntaxTree::SyntaxTree(Statements&& statements)
: statements(std::move(statements))
{
    tac = tac::Tac();
}



SyntaxTree::SyntaxTree(Tokens::TokenList& tokens)
{
    using namespace Tokens;

    if (tokens.first == nullptr)
    {
        // TODO implement errors

        return;
    }

    // start of script
    statements = Statements(); // linked list of Statement
    Statement* statement = nullptr;
    Token* token;

    // transform the TokenList to a list of Statement
    for (Token* tok = tokens.first; tok != nullptr; tok = tok->next)
    {

        // end of statement --> add statement to list of statements
        if (tok->type == TokenType::ENDS)
        {
            // don't add empty statements
            if (statement != nullptr && statement->root != nullptr)
            {
                token->next = nullptr;
                statements.add(statement);
                statement = nullptr;
                token = nullptr;
            }
            continue;
        }

        // first token of statement
        if (statement == nullptr)
        {
            tok->prev = nullptr;
            statement = new Statement(tok);
        }

        token = tok;

    }

}


const tac::Tac& SyntaxTree::getTac() const 
{
    return tac;
}


// returns the token with the highest priority in the statement
Tokens::Token* SyntaxTree::getHighestPriority(Tokens::Token* root) 
{
    using namespace Tokens;

    // check if first token of statement is null (empty statement)
    if (root == nullptr)
    {
        return nullptr;
    }

    for (Tokens::Token* token = root; token != nullptr; token = token->next)
    {   
        // evaluate the scope first
        if (isScope(token->opCode) && token->priority != 0)
        {
            return token;
        }

        if (token->priority > root->priority)
        {
            root = token;
        }
    }

    // return token with highest priority
    return root;
}


void SyntaxTree::parse()
{
    /*
        loop through every statement
        for every statement, parse it and build a tree out of it
        keep statement in outer scope to set Statements (linked list)'s
        last element to the last evaluated statement
    */
    Statement* statement;
    for (statement = statements.start; true; statement = statement->next)
    {
        parseStatement(statement);

        // check if next statement is nullptr and break
        // so not to set statements.end to nullptr (aka. cause segfault)
        if (statement->next == nullptr)
        {
            break;
        }

    }

    // set linked list's last element to the last evaluated statement
    statements.end = statement;

}


std::ostream& operator<<(std::ostream& stream, SyntaxTree const& tree)
{   
    stream << "Syntax Tree: {\n";

    for (Statement* statement = tree.statements.start; statement != nullptr; statement = statement->next)
    {
        stream << "\t" << *statement << "\n";
    }

    return stream << "}";
}

