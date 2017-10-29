#include <stdio.h>

Token_t* active_token;

int parse();
int NT_program();
int NT_head();
int NT_scope();
int NT_body();
int NT_func_dec();
int NT_func_def();
int NT_func();
int NT_parameter_list();
int NT_nextparam();
int NT_var_dec();
int NT_var_def();
int NT_statement();
int NT_condition();
int NT_loop();
int NT_assign();
int NT_return();
int NT_callfunc();
int NT_term_list();
int NT_next_term();
int NT_term();



int match(token_type expected_token_type) // @TODO check if possible to pass type as global
{
    if (active_token->type == expected_token_type)
    {
        active_token = get_token();
        if (active_token != NULL)
            return 1;

        printf("\n Unexpected token found. \n");
        return E_SYNTAX;
    }
    else
        return 0;
}


/*
 * MAIN START POINT
 */
int parse()
{


}

/*
 * GLOBAL program non-terminal
 */
int NT_program()
{

    NT_head();
    NT_scope();

    if (match(token_eof) != 1 )
        return E_SYNTAX;
    else
        return SUCCESS; // syntax is ok

}

int NT_head()
{
    //if ( first token is  First(NT_func_dec) ) @TODO make First/Follow table
        NT_func_dec();
        NT_head();
    //if ( first token is  First(NT_func_def) )
        NT_func_def();
        NT_head();
    // else ?
        return 1;
}

int NT_scope()
{
    if (match(token_eof) != 1 )
        return E_SYNTAX;

    NT_body();

    if (match(token_endscope) != 1 )
        return E_SYNTAX;
}

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

int NT_func_dec()
{
    if (match(token_declare) != 1 )
        return E_SYNTAX;

    NT_func();

    // else ?
        return 1; // move on
}

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

int NT_parameter_list()
{
    if (match(token_identifier) == 1 ) // !! inverse comparison
        NT_nextparam();
    else
        return 1; // move on
}

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

int NT_var_def()
{
    NT_var_dec();

    if (match(token_equals) != 1 )
        return E_SYNTAX;

    NT_exp(); // ! different parsing

}

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

int NT_assign()
{
    if (match(token_identifier) != 1 )
        return E_SYNTAX;

    if (match(token_equals) != 1 )
        return E_SYNTAX;
}

int NT_return()
{
    if (match(token_return) != 1 )
        return E_SYNTAX;

    NT_exp();

}

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

int NT_term_list()
{
    NT_term();
    NT_next_term();
}

int NT_next_term()
{

    if (match(token_comma) == 1 )
    {
        NT_term();
        NT_next_term();
    }

}

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
