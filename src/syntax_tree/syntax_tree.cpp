#include "syntax_tree.hh"


using namespace syntax_tree;



SyntaxTree::SyntaxTree(Statements&& statements)
: statements(std::move(statements))
{
	
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


// returns the token with the highest priority in the statement
Tokens::Token* SyntaxTree::getHighestPriority(Tokens::Token* root) 
{
	using namespace Tokens;

	// check for empty statements
	if (root == nullptr)
	{
		return nullptr;
	}

	for (Tokens::Token* token = root; token != nullptr; token = token->next)
	{   
		// evaluate the scope first since they are required as operands by certain operators
		if (isScope(token->opCode) && token->priority != 0)
		{
			return token;
		}

		if (token->priority > root->priority)
		{
			root = token;
		}
	}

	return root;
}


pvm::ByteCode SyntaxTree::parseToByteCode()
{
	using namespace symbol_table;

	byteList = pvm::ByteList();

	SymbolTable::init();

	// since this is the global scope, pop the symbols at the end
	parseToByteCode(DONT_POP_SCOPE);

	SymbolTable::clear();
}


void SyntaxTree::parseToByteCode(bool doPopScope)
{
	using namespace symbol_table;

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

		/*
			check if next statement is nullptr and break
			so not to set statements.end to nullptr (aka. cause segfault)
			this check has to be performed here in case of single-statement
			SyntaxTrees 
		*/
		if (statement->next == nullptr)
		{
			break;
		}

	}

	// set linked list's last element to the last evaluated statement
	statements.end = statement;

	// at the end of tree generation, transform it into Tac
	generateByteCode(doPopScope);

	// now that the byte code for the tree has been generated and the scope size determined,
	// add the push instruction at the beginning of the scope
	byteList.insertFisrt(new pvm::ByteNode(pvm::OpCode::PUSH_BYTES));
	byteList.insertFisrt(new pvm::ByteNode(SymbolTable::getScope()->localSymbolsSize));

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


void SyntaxTree::generateByteCode(bool doPopScope)
{
	/*
		- get all the stack-declared variables from the SymbolTable
		- add the pre-allocated stack variables to the tree's Tac's
		  declared symbols list
		- generate the Tac for the actual instructions
		- pop the local scope symbols from the stack
	*/

	using namespace symbol_table;
	using namespace Tokens;

	// get the current scope
	const Scope* scope = SymbolTable::getScope();

	// declare the local symbols in the new CodeBlock

	// generate Tac for the actual instructions
	for (Statement* statement = statements.start; statement != nullptr; statement = statement->next)
	{
		for (Token* token = statement->root; token != nullptr; token = token->next)
		{
			if (isOperator(token->opCode))
			{
				parseTokenOperator(token);
			}
		}
	}

	// pop the local scope symbols from the stack if required
	if (doPopScope)
	{
		//block->popSymbols(SymbolTable::getScope());
	}

}

