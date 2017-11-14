
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

        static eof_token_found = false;

        if (eof_token_found == false)
        {
            if (active_token->type == expected_token_type)
            {
                active_token = get_token();
                
                if (active_token->type == token_eof) 
                    eof_token_found = true;
                
                return true;
            }
            else
            {
                active_token = get_token();

                if (active_token->type == token_eof)
                    eof_token_found = true;

                return false;
            }
        }
        else 
        {
            // dont try to get to the next token
            raise_error(E_SYNTAX, "Unexpected END OF FILE ! ! !");
            return false; 
        }          
    }


/******************************************************************************
    MAIN STARTING POINT
    - calls the first non-terminal and checks for the required EOL token
    - generates first set of instructions
    - creates global symbol table
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

        match(token_declare);

        if (match(token_function) == false)
            raise_error(E_SYNTAX, "'Function' keyword expected at this point.");


        if (active_token->type == token_identifier)
        {
            // Insert(GST, active_token); // ??? something like this Insert(SymTable *table, Token_t *token);
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