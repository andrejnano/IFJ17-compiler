
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
    #include <errorcodes.h>
    #include <symtable.h>
    #include <token.h>
    #include <parser.h>

    // current token from input
    Token_t* active_token;


/******************************************************************************
    TOKEN MATCH TEST
    - just checks if active_token is the expected token in the syntax
        > returns TRUE if the token is valid
        > returns FALSE if the tokens is not valid
    - doesn't move to the next token, just informs
    - set of predefined conditions based of LL(1) grammar
 *****************************************************************************/

    bool match_test(token_type expected_token_type)
    {
        return (active_token->type == expected_token_type) ? true : false;
    }


/******************************************************************************
    TOKEN MATCH & MOVE
    - same as TOKEN MATCH TEST but moves the active token to the next token
    from input with get_token()
 *****************************************************************************/

    bool match_move(token_type expected_token_type)
    {
        if (active_token->type == expected_token_type)
        {
            active_token = get_token();
            if (active_token == NULL)
                ; // @TODO no_more_tokens flag on to exit in higher layers

            return true;
        }
        else
            return false;
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

        NT_program();

        if(!match_move(token_eol))
            return E_SYNTAX;


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

    int NT_program()
    {

        NT_head();
        NT_scope();

        if (match(token_eof) != 1 )
            return E_SYNTAX;
        else
            return SUCCESS; // syntax is ok

    }


/*****************************************************************************/

    int NT_head()
    {
        //if ( first token is  First(NT_func_dec) )
            NT_func_dec();
            NT_head();
        //if ( first token is  First(NT_func_def) )
            NT_func_def();
            NT_head();
        // else ?
            return 1;
    }


/*****************************************************************************/

    int NT_scope()
    {
        if (match(token_eof) != 1 )
            return E_SYNTAX;

        NT_body();

        if (match(token_endscope) != 1 )
            return E_SYNTAX;
    }


/*****************************************************************************/

    int NT_body()
    {
        //if ( first token is  First(NT_var_def) ) @TODO make First/Follow table
            NT_var_def();
            NT_body();
        //if ( first token is  First(NT_statement) )
            NT_statement();
            NT_body();
        // else ?
            return 1; // move on
    }


/*****************************************************************************/

    int NT_func_dec()
    {
        if (match(token_declare) != 1 )
            return E_SYNTAX;

        NT_func();

        // else ?
            return 1; // move on
    }


/*****************************************************************************/

    int NT_func_def()
    {
        NT_func();

        if (match(token_eol) != 1 )
            return E_SYNTAX;

        NT_body();

        if (match(token_end) != 1 )
            return E_SYNTAX;

        if (match(token_function) != 1 )
            return E_SYNTAX;

    }


/*****************************************************************************/

    int NT_func()
    {
        if (match(token_function) != 1 )
            return E_SYNTAX;

        if (match(token_identifier) != 1 )
            return E_SYNTAX;

        if (match(token_lbrace) != 1 )
            return E_SYNTAX;

        NT_parameter_list();

        if (match(token_rbrace) != 1 )
            return E_SYNTAX;

        if (match(token_as) != 1 )
            return E_SYNTAX;

        if (match(token_returntype) != 1 )
            return E_SYNTAX;
    }


/*****************************************************************************/

    int NT_parameter_list()
    {
        if (match(token_identifier) == 1 ) // !! inverse comparison
            NT_nextparam();
        else
            return 1; // move on
    }


/*****************************************************************************/

    int NT_nextparam()
    {
        if (match(token_comma) == 1 ) // !! inverse comparison
        {
            if (match(token_identifier) != 1 )
                return E_SYNTAX;

            NT_nextparam();
        }
        else
            return 1; // move on
    }


/*****************************************************************************/

    int NT_var_dec()
    {
        if (match(token_dim) != 1 )
            return E_SYNTAX;

        if (match(token_identifier) != 1 )
            return E_SYNTAX;

        if (match(token_as) != 1 )
            return E_SYNTAX;

        if (match(token_datatype) != 1 )
            return E_SYNTAX;
    }


/*****************************************************************************/

    int NT_var_def()
    {
        NT_var_dec();

        if (match(token_equals) != 1 )
            return E_SYNTAX;

        NT_exp(); // ! different parsing

    }


/*****************************************************************************/

    int NT_statement()
    {

        //if ( first token is  First(NT_callfunc) e.g. token_id)
            NT_callfunc();
        //else if ( first token is  First(NT_return) )
            NT_return();
        //else if ( first token is  First(NT_assign) )
            NT_assign();
        //else if ( first token is  First(NT_condition) )
            NT_condition();
        //else if ( first token is  First(NT_loop) )
            NT_loop();

        else if (match(token_eol) == 1 ) // !inverse comparison
        {
            NT_statement();
        }
        else
            return 1; // move on
    }


/*****************************************************************************/

    int NT_condition()
    {

        if (match(token_if) != 1 )
            return E_SYNTAX;

        NT_exp();

        if (match(token_then) != 1 )
            return E_SYNTAX;

        if (match(token_eol) != 1 )
            return E_SYNTAX;

        NT_statement();

        if (match(token_else) != 1 )
            return E_SYNTAX;

        NT_statement();

        if (match(token_endif) != 1 ) // ?? not sure
            return E_SYNTAX;

    }


/*****************************************************************************/

    int NT_loop()
    {
        if (match(token_do) != 1 )
            return E_SYNTAX;

        if (match(token_while) != 1 )
            return E_SYNTAX;

        NT_exp();

        if (match(token_eol) != 1 )
            return E_SYNTAX;

        NT_statement();

        if (match(token_loop) != 1 )
            return E_SYNTAX;

        // @TODO probably also loop end ?

    }


/*****************************************************************************/

    int NT_assign()
    {
        if (match(token_identifier) != 1 )
            return E_SYNTAX;

        if (match(token_equals) != 1 )
            return E_SYNTAX;
    }


/*****************************************************************************/

    int NT_return()
    {
        if (match(token_return) != 1 )
            return E_SYNTAX;

        NT_exp();
    }


/*****************************************************************************/

    int NT_callfunc()
    {

        NT_assign();

        if (match(token_identifier) != 1 )
            return E_SYNTAX;

        if (match(token_lbrace) != 1 )
            return E_SYNTAX;

        NT_term_list();

        if (match(token_rbrace) != 1 )
            return E_SYNTAX;

        if (match(token_eol) != 1 )
            return E_SYNTAX;

    }


/*****************************************************************************/

    int NT_term_list()
    {
        NT_term();
        NT_next_term();
    }


/*****************************************************************************/

    int NT_next_term()
    {

        if (match(token_comma) == 1 )
        {
            NT_term();
            NT_next_term();
        }
    }


/*****************************************************************************/

    int NT_term()
    {
        while(1)
        {
            if(match(token_integer))
                break;
            if(match(token_double))
                break;
            if(match(token_string))
                break;
            if(match(token_identifier))
                break;
            else
                break;
        }
    }
