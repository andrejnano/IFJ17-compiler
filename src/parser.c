
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
    @TODO dilema.. move aj ked je to nespravne ? alebo len vratit false a 
                    zostat na tom istom tokene
 *****************************************************************************/

    bool match(token_type expected_token_type)
    {
        if (active_token->type == expected_token_type)
        {
            active_token = get_token();
            return true;
        }
        else
        {
            active_token = get_token();
            return false;
        }
    }


/******************************************************************************
    MAIN STARTING POINT
    - calls the first non-terminal and checks for the required EOL token
    - generates first set of instructions
    - creates global symbol table
    - ...
 *****************************************************************************/

    int parse()
    {

        // @TODO create new symbol table
        // @TODO create new instructions vector/list/structure

        active_token = get_token();

        NT_Program();   // root nonterminal

        // @TODO clean up
        // @TODO finish the syntax control
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

        if (match(token_eof) == false)
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
        if (match(token_scope) == false)
            raise_error(E_SYNTAX, "Expected 'Scope' keyword not found.");
            
        NT_CompoundStmt();

        if (match(token_end) == false)
            raise_error(E_SYNTAX, "Expected 'End' keyword not found.");

        if (match(token_scope) == false)
            rraise_error(E_SYNTAX, "Expected 'Scope' keyword not found.");
    }


/*****************************************************************************/

    void NT_CompoundStmt()
    {
        switch(active_token->type)
        {
            case token_dim:         NT_VarDef(); break; // decide if declaration or definition inside definition
            case token_identifier:  NT_AssignStmt(); break;
            case token_return:      NT_ReturnStmt(); break;
            case token_input:       NT_InputStmt(); break;
            case token_print:       NT_IfStmt(); break;
            case token_do:          NT_WhileStmt(); break;
            default: return;    // epsilon rule 
        }

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        NT_CompoundStmt();
    }


/*****************************************************************************/
    // @TODO skombinovat s tym co robil Peto

    void NT_FunctionDec()
    {
        // probably dont even need to compare.. just match()
        if (match(token_declare) == false)
            raise_error(E_SYNTAX, "'Declare' keyword went missing.. ");

        if (match(token_function) == false)
            raise_error(E_SYNTAX, "'Function' keyword expected at this point.");

        if (active_token->type == token_identifier)
        {
            Insert(GST, active_token); // ??? something like this Insert(SymTable *table, Token_t *token);
            match(token_identifier);
        }

        if (match(token_lbrace) == false)
            raise_error(E_SYNTAX, "'(' was expected.");

        NT_ParamList();

        if (match(token_rbrace) == false)
            raise_error(E_SYNTAX, "')' was expected.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "'As' keyword was expected.");

        if (active_token->type == token_datatype)
        {
            // insert new function id with returny type of datatype
        }
    }


/*****************************************************************************/
    // @TODO skombinovat s tym co robil Peto
    void NT_FunctionDef()
    {
        if (match(token_function) == false)
            raise_error(E_SYNTAX, "'Function' keyword expected at this point.");

        
        if (active_token->type == token_identifier)
        {
            InsertNew
        }

        if (match(token_lbrace) == false)
            raise_error(E_SYNTAX, "'(' was expected.");

        NT_ParamList();

        if (match(token_rbrace) == false)
            raise_error(E_SYNTAX, "')' was expected.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "'As' keyword was expected.");

        if (match(token_datatype) == false)
            raise_error(E_SYNTAX, "')' was expected.");

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        NT_CompoundStmt();

        if (match(token_end) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        if (match(token_function) == false )
            raise_error(E_SYNTAX, "EOL expected at this point.");
    }



/*****************************************************************************/

    void NT_ParamList()
    {
        if (active_token->type == token_identifier)
            NT_Param();
        if (active_token->type == token_comma)
            NT_NextParam();

        // epsilon rule
    }


/*****************************************************************************/

    void NT_NextParam()
    {   
        if (active_token->type == token_comma)
        {   
            // just match(), no need to compare and raise error
            if (match(token_comma) == false)
                raise_error(E_SYNTAX, "Comma ',' expected in the parameter list.");

            NT_Param();
            NT_NextParam();
        }
        //  epsilon rule
    }

/*****************************************************************************/

    void  NT_Param()
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
        }
        // if no equals, just declare.. 
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

        NT_CompoundStmt();

        if (match(token_else) == false)
            raise_error(E_SYNTAX, "Keyword 'Else' expected.");

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        NT_CompoundStmt();

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

        NT_CompoundStmt();

        if (match(token_loop) == false)
            raise_error(E_SYNTAX, "Keyword 'Loop' expected");

    }


/*****************************************************************************/

    void NT_ReturnStmt()
    {
        if (match(token_return) == false)
            raise_error(E_SYNTAX, "Keyword 'Return' expected.");

        //@TODO generate return instruction

        NT_Expr();
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
    }


/*****************************************************************************/

    void NT_ExprList()
    {
        NT_Expr(); // CANNOT BE EMPTY EXPRESSIONS, at least one.. !! 

        if (match(token_semicolon) == false)
            raise_error(E_SYNTAX, "Semicolon ';' is missing. ");
        
        NT_NextExpr();
    }


/*****************************************************************************/
    //@TODO this probably doesn't need to be here.. just improve ExprList()
    void NT_NextExpr()
    {
        // if (is_expression(active_token)) // find out if the active token is a part of an expression...or otherwise said, token is not a keyword or a literal.. ? 
        {
            NT_ExprList();
        }
        //epsilon otherwise
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