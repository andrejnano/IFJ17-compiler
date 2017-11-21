
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
    #include "scanner.h"
    #include "parser.h"
    #include "symtable.h"

    // current token from input
    Token_t* active_token;
    SymbolTable_t *functions;

    extern FILE* source_code;


/******************************************************************************
 *****************************************************************************/

    /*
	* @brief Main parsing function
	* @param source_code Source file in IFJ17 language
	* @param output_code Destination file in IFJcode17 language
	*/
    void parse()
    {

        //@TODO Instruction List init
        //InstructionList_t InstList; // Instruction list for output code
        //IL_Init(&InstList);

        stl_init(&functions);

        // first token reading
        if (get_next_token(source_code, active_token) == false || active_token->type == token_eof)
        {
            free(active_token);
            scanner_free();
            raise_error(E_SYNTAX, "EOF at the beginning of file.");
        }
        else
        {
            // EVERYTHING IS OK HERE :)
            // root nonterminal
            NT_Program();
        }

        scanner_free();
        stl_clean_all(&functions);
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
    // @TODO skombinovat s tym co robil Peto

    void NT_FunctionDec()
    {

		match(token_declare);

		if (match(token_function) == false)
            raise_error(E_SYNTAX, "'Function' keyword expected at this point.");

        /*
            Function Declaration
            3 scenarios can occur

            1) Function was already declared
            2) Function was already defined
            3) Function was neither defined nor declared

            in case 1) raise ERROR & quit
                        if ( is_declared == TRUE ) 
    
            in case 2) raise ERROR & quit
                        if ( is_defined == TRUE )

            in case 3) create new symtable item and declare
                        if ( no item exists ) ...  is_declared = TRUE;

        */

        // declare a new symtable item.
        Metadata_t  new_function_metadata;
        char*       new_function_name = NULL;
        
        bool declaration_error = false;

        if (active_token->type == token_identifier)
        {
            // lookup the identifier in symtable...
            Metadata_t *function_metadata = stl_search(functions, active_token->value.c);

            // if the identifier is in symtable
            if (function_metadata)
            {
                if (function_metadata->is_defined)
                {
                    raise_error(E_SYNTAX, "Function with this name is already defined.");
                    declaration_error = true;
                }
                else if (function_metadata->is_declared)
                {
                    raise_error(E_SYNTAX, "Function with this name is already declared.");
                    declaration_error = true;
                }
            }
            else   // if the identifier in NOT in symtable
            {
                // create a new symtable item locally.
                new_function_metadata.is_declared = true;
                new_function_metadata.is_defined = false;
                new_function_metadata.parameters = NULL;

                // create copy of the string
                new_function_name = (char *) malloc( sizeof(char) * strlen(active_token->value.c) + 1);
                strcpy(active_token->value.c, new_function_name);

                match(token_identifier);

                if (match(token_lbrace) == false)
                    raise_error(E_SYNTAX, "'(' was expected.");

                // >>>>>>>>>>>>>>>
                //  PARAMETERS 
                // >>>>>>>>>>>>>>>

                Parameter_t *function_parameters = NULL;

                while(true)
                {
                    // in case of undefined symbol or no parameters
                    if (active_token->type != token_identifier)
                        break;

                    // create new Parameter
                    Parameter_t *new_parameter = (Parameter_t *) malloc(sizeof(Parameter_t));
                    new_parameter->name = active_token->value.c;
                    new_parameter->next = NULL;

                    match(token_identifier);

                    if (match(token_as) == false)
                        raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");
                    
                    // check if the token is one of the datatypes
                    if (is_datatype(active_token->type))
                    {
                        new_parameter->type = active_token->type;
                    }
                    else
                    {
                        raise_error(E_SYNTAX, "Expected datatype after a parameter.");
                    }

                    match(token_blank); // @TODO len sa posunut, zaruceny fail sice,.. pozriet co je blank

                    if (active_token->type == token_comma)
                    {
                        match(token_comma);

                        // append current parameter and continue with next one
                        if ( param_list_append(&function_parameters, new_parameter) )
                            continue;
                        else
                            free(new_parameter);
                    }
                    else if (active_token->type == token_rbrace)
                    {
                        param_list_append(&function_parameters, new_parameter);
                        break;
                    }
                    else
                    {
                        // some unexpected token
                        break;
                    }

                } // end of while loop

                new_function_metadata.parameters = function_parameters;

                // <<<<<<<<<<<<<<<<<<
                //  END OF PARAMETERS
                // <<<<<<<<<<<<<<<<<<

                if (match(token_rbrace) == false)
                    raise_error(E_SYNTAX, "')' was expected.");

                if (match(token_as) == false)
                    raise_error(E_SYNTAX, "'As' keyword was expected.");

                // check if the token is one of the datatypes
                if (is_datatype(active_token->type))
                {
                    new_function_metadata.type = active_token->type;
                }
                else
                {
                    raise_error(E_SYNTAX, "Expected datatype after a parameter.");
                    declaration_error = true;
                }

                match(token_blank); // just move

                // FINALLY add to symtable if there are no error during declaration
                if (!declaration_error && !compiler_error)
                {
                    stl_insert_top(functions, new_function_name, &new_function_metadata);
                    free(new_function_name);
                }
            }
        }
        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier was expected.");
        
        free(new_function_name);
    }


/*****************************************************************************/
    // @TODO skombinovat s tym co robil Peto
    
    void NT_FunctionDef()
    {

        // token_function token_identifier token_lbrace <ParameterList> token_rbrace token_as token_datatype
        // token_eol <CompoundStmt> token_end token_function

        /*
            3 scenarios can occur

            1) Function was already defined
            2) Function was already declared
            3) Function was neither defined nor declared

            in case 1) raise ERROR & quit
                        if ( is_defined == TRUE ) ... 
    
            in case 2) check if the parameters match the declaration, then push the body
                        if ( is_declared == TRUE ) ...  is_defined = TRUE;

            in case 3) declare and define a new symtable item.
                        if ( no item exists ) ...  is_declared = TRUE ; is_defined = TRUE;

        

        match(token_function);
       

        if (active_token->type == token_identifier)
        {
            // lookup the identifier in symtable...
			node_val_t *function_metadata = STL_Search(functions, active_token->value.c);

            // if the identifier is in symtable
            if ( function_metadata )
            {

                // if the function is already defined
                if (function_metadata->is_defined)
                {
                    raise_error(E_SYNTAX, "Function is already defined.");
                    return; // or better just break off this scope  
                }
                else 
                if (function_metadata->is_declared)
                {
                    // check the parameters if they are consistent

                    node_val_t new_function_metadata;
                    tArglist *param_list = NULL;
                    // ...

                }
            }
            else
            {
                // declare and define a new symtable item.
                node_val_t new_function_metadata;
                // ...
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
            */
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
            raise_error(E_SYNTAX, "Expected 'Scope' keyword not found.");
    }



/*****************************************************************************/

    void NT_CompoundStmt()
    {

        switch (active_token->type)
        {
        case token_dim:
            NT_VarDef();
            break; // keyword 'Dim'
        case token_identifier:
            NT_AssignStmt();
            break; // identifier
        case token_return:
            NT_ReturnStmt();
            break; // keyword 'Return'
        case token_input:
            NT_InputStmt();
            break; // keyword 'Input'
        case token_print:
            NT_PrintStmt();
            break; // keyword 'Print'
        case token_if:
            NT_IfStmt();
            break; // keyword 'If'
        case token_do:
            NT_WhileStmt();
            break; // keyword 'Do'
        default:
            return; // epsilon rule
        }

        if (match(token_eol) == false) // end of line
            raise_error(E_SYNTAX, "EOL expected at this point.");

        NT_CompoundStmt();
    }





// /*****************************************************************************/

//     void NT_VarDec()
//     {
//         if (match(token_dim) == false)
//             raise_error(E_SYNTAX, "Keyword 'Dim' was expected.");

//         // @TODO >>>>>>>>
//         // Generate instruction, declare inside symtable.. etc. 
//         if (match(token_identifier) == false)
//             raise_error(E_SYNTAX, "Identifier expected and not found.");

//         if (match(token_as) == false)
//             raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");

//         // @TODO >>>>>>>>
//         // Update inside symtable ? & generate instruction probz.
//         // if (match(token_datatype) == false)
//         //     raise_error(E_SYNTAX, "Data type missing in the parameter.");
//     }


// /*****************************************************************************/

//     void NT_VarDef()
//     {
//         // NT_VarDec();

//         // if (active_token->type == token_equal)
//         // {
//         //     if (match(token_equals) == false)
//         //         raise_error(E_SYNTAX, "Assignment operator '=' expected.");
            
//         //     NT_Expr();

//         //     // generate instruction for definition
//         //     return;
//         // }
//         // if no equals, just declare.. 
//         // finish generating instructions just for declaration
//     }

// /*****************************************************************************/

//     void NT_AssignStmt()
//     {
//         // @TODO >>>>>>>>
//         // Generate instruction, declare inside symtable.. etc.

//         if (match(token_identifier) == false)
//             raise_error(E_SYNTAX, "Identifier expected and not found.");

//         // if (match(token_equals) == false)
//         //     raise_error(E_SYNTAX, "Assignment operator '=' expected.");

//         NT_Expr();
//     }

// /*****************************************************************************/

//     void NT_IfStmt()
//     {

//         if (match(token_if) == false)
//             raise_error(E_SYNTAX, "Keyword 'If' expected.");

//         NT_Expr();

//         if (match(token_then) == false)
//             raise_error(E_SYNTAX, "Keyword 'Then' expected.");

//         if (match(token_eol) == false)
//             raise_error(E_SYNTAX, "EOL expected at this point.");

//         // -----------------
//         // NEW LOCAL SCOPE
//         // -----------------

//         NT_CompoundStmt();

//         // ------------------
//         // END OF LOCAL SCOPE
//         // ------------------

//         if (match(token_else) == false)
//             raise_error(E_SYNTAX, "Keyword 'Else' expected.");

//         if (match(token_eol) == false)
//             raise_error(E_SYNTAX, "EOL expected at this point.");

//         // -----------------
//         // NEW LOCAL SCOPE
//         // -----------------

//         NT_CompoundStmt();

//         // ------------------
//         // END OF LOCAL SCOPE
//         // ------------------

//         if (match(token_end) == false)
//             raise_error(E_SYNTAX, "Keyword 'End' expected.");
        
//         if (match(token_if) == false)
//             raise_error(E_SYNTAX, "Keyword 'If' expected.");
//     }


// /*****************************************************************************/

//     void NT_WhileStmt()
//     {
//         if (match(token_do) == false)
//             raise_error(E_SYNTAX, "Keyword 'Do' expected.");

//         if (match(token_while) == false)
//             raise_error(E_SYNTAX, "Keyword 'While' expected.");

//         NT_Expr();

//         if (match(token_eol) == false)
//             raise_error(E_SYNTAX, "EOL expected.");

//         // -----------------
//         // NEW LOCAL SCOPE
//         // -----------------

//         NT_CompoundStmt();

//         // ------------------
//         // END OF LOCAL SCOPE
//         // ------------------

//         if (match(token_loop) == false)
//             raise_error(E_SYNTAX, "Keyword 'Loop' expected");

//     }


// /*****************************************************************************/

//     void NT_ReturnStmt()
//     {
//         if (match(token_return) == false)
//             raise_error(E_SYNTAX, "Keyword 'Return' expected.");

//         NT_Expr();

//         // generate instructions
//     }


// /*****************************************************************************/

//     void NT_InputStmt()
//     {
//         if (match(token_input) == false)
//             raise_error(E_SYNTAX, "Keyword 'Input' expected.");

//         if (active_token->type == token_identifier)
//         {
//             // check if identifier is declared
//             // update the identifier
//             // generate instruction
//         }

//         if (match(token_identifier) == false)
//             raise_error(E_SYNTAX, "Identifier is expected right after 'Input' keyword.");
//     }

// /*****************************************************************************/

//     void NT_PrintStmt()
//     {
//         if (match(token_print) == false)
//             raise_error(E_SYNTAX, "Keyword 'Print' expected.");
        
//         NT_ExprList();

//         // for each expression generate a print call inst. with it
//     }


// /*****************************************************************************/

//     void NT_ExprList()
//     {
//         NT_Expr(); // CANNOT BE EMPTY EXPRESSIONS, at least one.. !! 

//         if (match(token_semicolon) == false)
//             raise_error(E_SYNTAX, "Semicolon ';' is missing. ");
        
//         if (is_expression(active_token)) // somehow do this test... 
//             NT_ExprList();
//         // epsilon rule otherwise
//     }


// /*****************************************************************************/

//     void NT_TermList()
//     {
//         // if the term list is empty end right brace is the next token..
//         if (active_token->type == token_rbrace) 
//             return;
        
//         switch (active_token->type)
//         {
//             case token_integer: 
//             {
//                 // do something
//                 match(token_integer);
//                 break;
//             }
//             case token_double:
//             {
//                 // do something
//                 match(token_double);
//                 break;
//             }
//             case token_string:
//             {
//                 // do something
//                 match(token_string);
//                 break;
//             }
//             case token_identifier:
//             {
//                 // do something
//                 match(token_identifier);
//                 break;
//             }

//             default: 
//             {
//                 raise_error(E_SYNTAX, "Unexpected term inside the term list.");
//                 active_token = get_token();
//                 return;
//             }
//         }

//         if (active_token->type == token_comma)
//         {
//             match(token_comma);
//             NT_TermList();
//         }
//     }


// /*****************************************************************************/

//     void NT_CallExpr()
//     {

//         if (active_token->type == token_identifier)
//         {
//             // do something
//             match(token_identifier);
//         }

//         if (match(token_lbrace) == false)
//             raise_error(E_SYNTAX, "Left brace '(' expected.");

//         NT_TermList();

//         if (match(token_rbrace) == false)
//             raise_error(E_SYNTAX, "Right brace ')' expected.");
        
//     }


// /*****************************************************************************/

//     void NT_Expr()
//     {
//         // MAGIC HAPPENS HERE.. 
//     }

