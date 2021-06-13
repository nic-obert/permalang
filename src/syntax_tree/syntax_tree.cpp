#include "syntax_tree.hh"
#include "symbol_table.hh"


using namespace syntax_tree;
using namespace symbol_table;


SyntaxTree::SyntaxTree()
: byteList(), statements()
{
	
}


SyntaxTree::SyntaxTree(Statements&& statements)
: statements(std::move(statements))
{
	
}



SyntaxTree::SyntaxTree(Tokens::TokenList& tokens)
{
	if (tokens.first == nullptr)
	{
		// TODO implement errors

		return;
	}

	// start of script
	statements = Statements(); // linked list of Statement
	Statement* statement = nullptr;
	Tokens::Token* token;

	// transform the TokenList to a list of Statement
	for (Tokens::Token* tok = tokens.first; tok != nullptr; tok = tok->next)
	{

		// end of statement --> add statement to list of statements
		if (tok->type == Tokens::TokenType::ENDS)
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


Tokens::Token* SyntaxTree::getHighestPriority(Tokens::Token* root) 
{
	// empty statements are checked by the calling function

	for (Tokens::Token* token = root; token != nullptr; token = token->next)
	{   

		// don't permorm further checks or operations, 0 priority will never be executed
		if (token->priority == 0)
		{
			continue;
		}

		// evaluate function declaration operators first since they must have higher priority
		// than their parameters
		if (token->opCode == OpCodes::FUNC_DECLARARION)
		{
			return token;
		}

		if (token->priority > root->priority)
		{
			root = token;
		}

		// do not search for tokens to evaluate past the scope
		// so to prevent a scope's content from being evaluated before
		// the scope operator
		if (token->opCode == OpCodes::PUSH_SCOPE)
		{
			break;
		}
	}

	return root;
}


pvm::ByteCode SyntaxTree::parseToByteCode()
{
	byteList = pvm::ByteList();

	SymbolTable::init();

	// since this is the global scope, pop the symbols at the end
	parseToByteCodePrivate();

	SymbolTable::clear();

	// add the last exit instruction to the byteList
	byteList.add(new pvm::ByteNode(pvm::OpCode::EXIT));
	byteList.add(new pvm::ByteNode(0, 1));

	return byteList.toByteCode();
}


void SyntaxTree::parseToByteCodePrivate()
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

	const size_t localSymbolsSize = SymbolTable::getScope()->localSymbolsSize;

	// don't add push instructions if there's nothing to push
	if (localSymbolsSize != 0)
	{
		byteList.add(new pvm::ByteNode(pvm::OpCode::PUSH_BYTES));
		byteList.add(new pvm::ByteNode(localSymbolsSize, 8));
	}

	// at the end of tree generation, transform it into byte code
	generateByteCode();

	// don't pop form the stack if nothing was added in the first place
	if (localSymbolsSize != 0)
	{
		byteList.add(new pvm::ByteNode(pvm::OpCode::POP));	
		byteList.add(new pvm::ByteNode(localSymbolsSize, 8));
	}

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


void SyntaxTree::generateByteCode()
{
	using namespace Tokens;

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
}

