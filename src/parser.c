
/******************************************************************************
 *    PARSER / SYNTAX ANALYZER
 *   input -> stream of Token_t* ( token pointers )
 *  output -> vector/list of Intruction_t* ( generated instruction pointers)
 *
 *  @TODO add instruction data structure and operations + vector of intructions
 *  @TODO add expression parsing
 *  @TODO define all semantic checks
 *  @TODO create a parse table with First() / Follow() tokens defined for each
 *        non-terminal or define First() and Follow() functions...
 *
 *****************************************************************************/

    // std library
    #include <stdio.h>
    #include <stdbool.h>

    // project headers
    #include "errorcodes.h"
    #include "symtable.h"
    #include "token.h"
    #include "parser.h"

    // current token from input
    Token_t* active_token;


/******************************************************************************
    TOKEN MATCH 
 *****************************************************************************/


	/*
	 * \brief	Checks if the current token is the expected one.
	 * 			If it is. It either frees the token and its memory
	 * 			or just moves on. In case of EOF, the function gets
	 * 			blocked for futher token getting. Just prints the error message.
	 * \param	expected_token_type The expected token by syntax
	 * \return	bool value, wether the token was found or not
	 * 			
	 * 			@TODO refactor a bit
	 */

    bool match(token_type expected_token_type)
    {

        // guard flag that, if switched on, forbids further reading from scanner
		// and therefore prevents wrong memory access.
		// switches on when at least one of these events happen:
		//	1) the scanner returns false, signaling erro
		//  2) the token is EOF, which is unexpected until the end of program
		// --------------------------------------------------------------------
        static bool block_scanner = false;


		if (block_scanner)
		{
            raise_error(E_SYNTAX, "Unexpected END OF FILE ! ! !");
            return false;

			// not exactly right, @TODO change this
			if (expected_token_type == token_eof)
			{
				return true;
			}
		}
		else
		{
			// if the token exists and its type is the expected type
			if (active_token && active_token->type == expected_token_type)
			{
				// if the token is string, free the char array
				if (active_token->type == token_val_string)
					free(active_token->value.c);

				// free the memory allocated by token
                free(active_token);

				// if error occurs in scanner,
				if (get_next_token(active_token) == false || active_token->type == token_eof)
				{
					block_scanner = true;
					printf("\n[DEBUG]: from match() > error in scanner or EOF token.\n"); // remove later
				}

				return true;
			}
            else
            {
				// if the token is string, free the char array
				if (active_token->type == token_val_string)
					free(active_token->value.c);

				// free the memory allocated by token
				free(active_token);

				// if error occurs in scanner,
				if (get_next_token(active_token) == false || active_token->type == token_eof)
				{
					block_scanner = true;
					printf("\n[DEBUG]: from match() > error in scanner or EOF token.\n"); // remove later
				}

				return false;
            }
        }
    }



/******************************************************************************
 *****************************************************************************/

	/*
	* @brief Main parsing function
	* @param source_code Source file in IFJ17 language
	* @param output_code Destination file in IFJcode17 language
	*/

		tSymbolTable *GST;

		int parse(FILE * source_code, FILE * output_code)
		{

			//@TODO Instruction List init
			//InstructionList_t InstList; // Instruction list for output code
			//IL_Init(&InstList);

			ST_Init(&GST);
			new_context(&GST);

			// first token reading
			if (get_next_token(active_token) == false || active_token->type == token_eof)
			{
				free(active_token);
				scanner_free();
				return compiler_error;
			}
			else
			{
				// EVERYTHING IS OK HERE :)

				// root nonterminal
				NT_Program();
			}

			scanner_free();
			STL_clean_up(&GST);
			return SUCCESS;
    }


/******************************************************************************
    FUNCTIONS FOR EVERY NON-TERMINAL
    - check if token on input is the expected one
    - or calls another non-terminal function
    - @TODO somehow set error flag if syntax is not valid, however dont stop
            the program from running, display errors at the end.
    - @TODO if the nonterminal has a local scope, then create new symbol table
            instance and semantically check from this table.
            (otherwise use global table)
 *****************************************************************************/

    void NT_Program()
    {

        NT_Head();
        NT_Scope();

        if (match(token_eof) == false)  // end of file
            raise_error(E_SYNTAX, "EOF expected after End Scope, not found.");
    }


/*****************************************************************************/

    void NT_Head()
    {
        if (active_token->type == token_declare)
        {
			NT_FunctionDec();
            NT_Head();
        }
        else 
        if (active_token->type == token_function)
        {
			NT_FunctionDef();
			NT_Head();
        }
        // epsilon rule is OK here
    }


/*****************************************************************************/

    void NT_Scope()
    {

        if (match(token_scope) == false) // keyword 'Scope'
            raise_error(E_SYNTAX, "Expected 'Scope' keyword not found.");

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------
        
            // create new symtable
            // assign symtable pointer to this local table, or make a linked list
            // of tables, change the pointer of the first to this local

            NT_CompoundStmt();


        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        if (match(token_end) == false) // keyword 'End'
            raise_error(E_SYNTAX, "Expected 'End' keyword not found.");

        if (match(token_scope) == false) // keyword 'Scope'
            rraise_error(E_SYNTAX, "Expected 'Scope' keyword not found.");
    }


/*****************************************************************************/

    void NT_CompoundStmt()
    {

        switch(active_token->type)
        {
            case token_dim:         NT_VarDef(); break;     // keyword 'Dim'
            case token_identifier:  NT_AssignStmt(); break; // identifier
            case token_return:      NT_ReturnStmt(); break; // keyword 'Return'
            case token_input:       NT_InputStmt();  break; // keyword 'Input'
            case token_print:       NT_PrintStmt(); break;  // keyword 'Print'
            case token_if:          NT_IfStmt(); break;     // keyword 'If'
            case token_do:          NT_WhileStmt(); break;  // keyword 'Do'
            default: return;        // epsilon rule 
        }

        if (match(token_eol) == false) // end of line
            raise_error(E_SYNTAX, "EOL expected at this point.");

        NT_CompoundStmt();
    }


/*****************************************************************************/
    // @TODO skombinovat s tym co robil Peto

    void NT_FunctionDec()
    {

		node_val_t *val;

		match(token_declare);

		if (match(token_function) == false)
            raise_error(E_SYNTAX, "'Function' keyword expected at this point.");

		// only process the token if it is an identifier
        if (active_token->type == token_identifier)
        {

			//  
			val = STL_Search(functions, active_token->value.c);

			// ak nieje v tabulke symbolov
			if (val == NULL)
			{
				char * new_function_name;

				// skopirovat string hodnotu tokenu
				active_token->value.c

				// vytvorit novu deklaraciu
				node_val_t new_declaration;

				// set new val
				val = STL_Insert();
			}
		}
        
        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier expected.");


        if (match(token_lbrace) == false)
            raise_error(E_SYNTAX, "'(' was expected.");

        NT_ParamList();

        if (match(token_rbrace) == false)
            raise_error(E_SYNTAX, "')' was expected.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "'As' keyword was expected.");


        if (active_token->type == token_datatype)
        {
            // something with symtable probably.. 
        }

        if (match(token_datatype) == false)
            raise_error(E_SYNTAX, "Data type expected at this point.");
    }


/*****************************************************************************/
    // @TODO skombinovat s tym co robil Peto
    
    void NT_FunctionDef()
    {

       match(token_function);
       

        if (active_token->type == token_identifier)
        {
            // lookup the identifier in symtable...

			node_val_t declaration_check = STL_Search(functions, active_token->value.c);
		}

        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier expected.");


        if (match(token_lbrace) == false)
            raise_error(E_SYNTAX, "'(' was expected.");

        NT_ParamList();

        if (match(token_rbrace) == false)
            raise_error(E_SYNTAX, "')' was expected.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "'As' keyword was expected.");

        // @TODO do semantics check if datatype matches the one in symtable
        if (match(token_datatype) == false)
            raise_error(E_SYNTAX, "Data type was expected.");

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------

            NT_CompoundStmt();

        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        if (match(token_end) == false)
            raise_error(E_SYNTAX, "Keyword 'End' expected.");

        if (match(token_function) == false )
            raise_error(E_SYNTAX, "Keyword 'Function' expected.");
    }



/*****************************************************************************/

    void NT_ParamList()
    {
        if (active_token->type == token_identifier)
        {
            NT_Param();

            if (active_token->type == token_comma)
                NT_NextParam();
        }
        // epsilon rule
    }


/*****************************************************************************/

    void NT_NextParam()
    {   
        if (active_token->type == token_comma)
        {   
            match(token_comma);

            NT_Param();
            NT_NextParam();
        }
        //  epsilon rule
    }

/*****************************************************************************/

    void NT_Param()
    {   
        // @TODO >>>>>>>> 
        // generate instruction to push identifier to stack or something.. 
        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier expected and not found.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");

        // @TODO >>>>>>>>
        // Generate instruction, convert or something..
        if (match(token_datatype) == false)
            raise_error(E_SYNTAX, "Data type missing in the parameter.");

    }
/*****************************************************************************/

    void NT_VarDec()
    {
        if (match(token_dim) == false)
            raise_error(E_SYNTAX, "Keyword 'Dim' was expected.");

        // @TODO >>>>>>>>
        // Generate instruction, declare inside symtable.. etc. 
        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier expected and not found.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");

        // @TODO >>>>>>>>
        // Update inside symtable ? & generate instruction probz.
        if (match(token_datatype) == false)
            raise_error(E_SYNTAX, "Data type missing in the parameter.");
    }


/*****************************************************************************/

    void NT_VarDef()
    {
        NT_VarDec();

        if (active_token->type == token_equals)
        {
            if (match(token_equals) == false)
                raise_error(E_SYNTAX, "Assignment operator '=' expected.");
            
            NT_Expr();

            // generate instruction for definition
            return;
        }
        // if no equals, just declare.. 
        // finish generating instructions just for declaration
    }

/*****************************************************************************/

    void NT_AssignStmt()
    {
        // @TODO >>>>>>>>
        // Generate instruction, declare inside symtable.. etc.

        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier expected and not found.");

        if (match(token_equals) == false)
            raise_error(E_SYNTAX, "Assignment operator '=' expected.");

        NT_Expr();
    }

/*****************************************************************************/

    void NT_IfStmt()
    {

        if (match(token_if) == false)
            raise_error(E_SYNTAX, "Keyword 'If' expected.");

        NT_Expr();

        if (match(token_then) == false)
            raise_error(E_SYNTAX, "Keyword 'Then' expected.");

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------

        NT_CompoundStmt();

        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        if (match(token_else) == false)
            raise_error(E_SYNTAX, "Keyword 'Else' expected.");

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------

        NT_CompoundStmt();

        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        if (match(token_end) == false)
            raise_error(E_SYNTAX, "Keyword 'End' expected.");
        
        if (match(token_if) == false)
            raise_error(E_SYNTAX, "Keyword 'If' expected.");
    }


/*****************************************************************************/

    void NT_WhileStmt()
    {
        if (match(token_do) == false)
            raise_error(E_SYNTAX, "Keyword 'Do' expected.");

        if (match(token_while) == false)
            raise_error(E_SYNTAX, "Keyword 'While' expected.");

        NT_Expr();

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected.");

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------

        NT_CompoundStmt();

        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        if (match(token_loop) == false)
            raise_error(E_SYNTAX, "Keyword 'Loop' expected");

    }


/*****************************************************************************/

    void NT_ReturnStmt()
    {
        if (match(token_return) == false)
            raise_error(E_SYNTAX, "Keyword 'Return' expected.");

        NT_Expr();

        // generate instructions
    }


/*****************************************************************************/

    void NT_InputStmt()
    {
        if (match(token_input) == false)
            raise_error(E_SYNTAX, "Keyword 'Input' expected.");

        if (active_token->type == token_identifier)
        {
            // check if identifier is declared
            // update the identifier
            // generate instruction
        }

        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier is expected right after 'Input' keyword.");
    }

/*****************************************************************************/

    void NT_PrintStmt()
    {
        if (match(token_print) == false)
            raise_error(E_SYNTAX, "Keyword 'Print' expected.");
        
        NT_ExprList();

        // for each expression generate a print call inst. with it
    }


/*****************************************************************************/

    void NT_ExprList()
    {
        NT_Expr(); // CANNOT BE EMPTY EXPRESSIONS, at least one.. !! 

        if (match(token_semicolon) == false)
            raise_error(E_SYNTAX, "Semicolon ';' is missing. ");
        
        if (is_expression(active_token)) // somehow do this test... 
            NT_ExprList();
        // epsilon rule otherwise
    }


/*****************************************************************************/

    void NT_TermList()
    {
        // if the term list is empty end right brace is the next token..
        if (active_token->type == token_rbrace) 
            return;
        
        switch (active_token->type)
        {
            case token_integer: 
            {
                // do something
                match(token_integer);
                break;
            }
            case token_double:
            {
                // do something
                match(token_double);
                break;
            }
            case token_string:
            {
                // do something
                match(token_string);
                break;
            }
            case token_identifier:
            {
                // do something
                match(token_identifier);
                break;
            }

            default: 
            {
                raise_error(E_SYNTAX, "Unexpected term inside the term list.");
                active_token = get_token();
                return;
            }
        }

        if (active_token->type == token_comma)
        {
            match(token_comma);
            NT_TermList();
        }
    }


/*****************************************************************************/

    void NT_CallExpr()
    {

        if (active_token->type == token_identifier)
        {
            // do something
            match(token_identifier);
        }

        if (match(token_lbrace) == false)
            raise_error(E_SYNTAX, "Left brace '(' expected.");

        NT_TermList();

        if (match(token_rbrace) == false)
            raise_error(E_SYNTAX, "Right brace ')' expected.");
        
    }


/*****************************************************************************/

    void NT_Expr()
    {
        // MAGIC HAPPENS HERE.. 
    }