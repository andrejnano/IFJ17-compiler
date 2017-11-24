
/******************************************************************************
 *    PARSER / SYNTAX ANALYZER
 *   source_code -> stream of Token_t* ( token pointers )
 *  output_code -> vector/list of Intruction_t* ( generated instruction pointers)
 *
 *  @TODO add instruction data structure and operations + vector of intructions
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
    #include "expression.h"
    #include "instructions.h"



    // current token from scanner
    Token_t* active_token;

    // symbol tables pointers
    SymbolTable_t *functions;
    SymbolTable_t *variables;

    // function return type
    int function_return_datatype = 0;

    // compiler error status (or success)
    extern int compiler_error;

    // source and output stream
    extern FILE *source_code;
    extern FILE *output_code;

    char *tmp_name; //uniqe variable name
    int tmp_cnt;
    void next_tmp_name();

/******************************************************************************
 *****************************************************************************/

    /*
	* @brief Main parsing function
	*/
    void parse()
    {
        // allocate and initialize new token on heap
        active_token = create_empty_token();

        if (!active_token)
            exit(7);

        //@TODO Instruction List init
        //InstructionList_t InstList; // Instruction list for output_code code
        //IL_Init(&InstList);

        // init the symtable lists
        stl_init(&functions);
        stl_init(&variables);

        scanner_init();

        // first token reading
        if (get_next_token(source_code, active_token) == false)
        {
            //free(active_token);
            raise_error(E_SYNTAX, "EOF at the beginning of file.");
        }
        else
        {
            // EVERYTHING IS OK HERE :)
            #ifdef DEBUG
                printf("\n\t--- START---\n\n");
            #endif

            stl_push(&functions); // initialize functions symtable

            NT_Program(); // root nonterminal
        }

        free(active_token);
        
        scanner_free();

        // clean and close the symtable lists
        stl_clean_all(&variables);
        stl_clean_all(&functions);
    }


/******************************************************************************
    FUNCTIONS FOR EVERY NON-TERMINAL
    - check if token on source_code is the expected one
    - or calls another non-terminal function
 *****************************************************************************/

    void NT_Program()
    {
        tmp_cnt = 0;//inicialization of temporary names generator
        tmp_name = malloc(sizeof(char)*16);

        add_inst(".IFJcode17", i_null, NULL, i_null, NULL, i_null, NULL);
        add_inst("jump main", i_null, NULL, i_null, NULL, i_null, NULL);

        NT_Head();
        NT_Scope();

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        if (match(token_eof) == false)  // end of file
				raise_error(E_SYNTAX, "EOF expected after End Scope, not found.");
			if (active_token->type == token_identifier && active_token->value.c)
				free(active_token->value.c);
    }


/*****************************************************************************/

    void NT_Head()
    {

        if (active_token->type == token_declare)
        {
            NT_FunctionDec();
            if (match(token_eol) == false)
                raise_error(E_SYNTAX, "EOL expected at this point.");
            NT_Head();
        }
        else 
        if (active_token->type == token_function)
        {
			NT_FunctionDef();
            if (match(token_eol) == false)
                raise_error(E_SYNTAX, "EOL expected at this point.");
            NT_Head();
        }
        // epsilon rule is OK here
    }


/*****************************************************************************/

    /**
     * @brief nonterminal function for Function Declaration
     * 
     *  1) Function was already declared -> raise ERROR & quit
     *  2) Function was already defined  -> raise ERROR & quit
     *  3) Function was neither defined nor declared
     *      -> create new symtable item and declare
     * 
     */
    
    void NT_FunctionDec()
    {

		match(token_declare);
        
        if (match(token_function) == false)
            raise_error(E_SYNTAX, "'Function'xpected at this point.");
       

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
                    raise_error(E_SEM_DEF, "Function with this name is already defined.");
                    declaration_error = true;
                }
                else if (function_metadata->is_declared)
                {
                    raise_error(E_SEM_DEF, "Function with this name is already declared.");
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
                strcpy(new_function_name, active_token->value.c);

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
                    Parameter_t new_parameter;
						  new_parameter.name = (char *)malloc(strlen(active_token->value.c) + 1);
						  strcpy(new_parameter.name, active_token->value.c);
                    new_parameter.next = NULL;

                    match(token_identifier);

                    if (match(token_as) == false)
                        raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");
                    
                    // check if the token is one of the datatypes
                    if (is_datatype(active_token->type))
                    {
                        new_parameter.type = active_token->type;
                        match(active_token->type);
                    }
                    else
                    {
                        raise_error(E_SYNTAX, "Expected datatype after a parameter.");
                        match(token_blank); // @TODO len sa posunut, zaruceny fail sice,..
                    }

        
                    if (active_token->type == token_comma)
                    {
                        match(token_comma);
                       
                        // append current parameter and continue with next one
                        if ( param_list_append(&function_parameters, &new_parameter) )
                            continue;
                        else
                        {
                            declaration_error = true;
                        }
                            
                    }
                    else if (active_token->type == token_rbrace)
                    {
                        if(param_list_append(&function_parameters, &new_parameter))
                            break;
                        else
                        {
                            declaration_error = true;
                            break;
                        }
                            
                    }
                    else
                    {
                        declaration_error = true;
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
                    match(active_token->type);
                }
                else
                {
                    raise_error(E_SYNTAX, "Expected datatype after a parameter.");
                    declaration_error = true;
                    match(token_blank); // @TODO len sa posunut, zaruceny fail sice,..
                }

                // FINALLY add to symtable if there are no error during declaration
                if (!declaration_error) //&& !compiler_error)
                {
                    stl_insert_top(functions, new_function_name, &new_function_metadata);
                }
                free(new_function_name);
                return;
            }
        }
         // if identifier was not found, but we dont want to ruin syntax of the whole statement
        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Function identifier expected.");

        if (match(token_lbrace) == false)
            raise_error(E_SYNTAX, "'(' was expected.");

        NT_ParamList();

        if (match(token_rbrace) == false)
            raise_error(E_SYNTAX, "')' was expected.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "'As' keyword was expected.");

        if (is_datatype(active_token->type))
        {
            match(active_token->type);
        }
        else
        {
            raise_error(E_SYNTAX, "Expected datatype after a parameter.");
            match(token_blank); // just move
        }
    }



/*****************************************************************************/

    
    void NT_FunctionDef()
    {
        /*
            1) Function was already defined
            2) Function was already declared
				3) Function was neither defined nor declared
				
            in case 1) raise ERROR & quit
                        if ( is_defined == TRUE ) ... 
    
            in case 2) check if the parameters match the declaration, then push the body
								if ( is_declared == TRUE ) ...  is_defined = TRUE;
								
            in case 3) declare and define a new symtable item.
                        if ( no item exists ) ...  is_declared = TRUE ; is_defined = TRUE;
        */

        if (match(token_function) == false)
            raise_error(E_SYNTAX, "'Function' keyword expected at this point.");

        // declare a new symtable item.
        Metadata_t new_function_metadata;
        char *new_function_name = NULL;
        bool definition_error = false;


        if (active_token->type == token_identifier)
        {
            // lookup the identifier in symtable...
            Metadata_t *function_metadata = stl_search(functions, active_token->value.c);

            // function is in symtable
            if (function_metadata)
            {

                if (function_metadata->is_defined)
                {
                    raise_error(E_SEM_DEF, "Function with this name is already defined.");
                    definition_error = true;
                }
                else if (function_metadata->is_declared)
                {
                    // check the parameters
                    // if they match push the body

                    match(token_identifier);

                    if (match(token_lbrace) == false)
                        raise_error(E_SYNTAX, "'(' was expected.");

                    // >>>>>>>>>>>>>>>
                    //  PARAMETERS
                    // >>>>>>>>>>>>>>>

                    Parameter_t *function_parameters = NULL;

                    while (true)
                    {
                        // in case of undefined symbol or no parameters
                        if (active_token->type != token_identifier)
                            break;

                        // create new Parameter
                        Parameter_t new_parameter;
                        new_parameter.name = (char *)malloc(sizeof(char) * strlen(active_token->value.c) + 1);
						strcpy(new_parameter.name, active_token->value.c);
                        new_parameter.next = NULL;

                        match(token_identifier);

                        if (match(token_as) == false)
                            raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");

                        // check if the token is one of the datatypes
                        if (is_datatype(active_token->type))
                        {
                            new_parameter.type = active_token->type;
                            match(active_token->type);
                        }
                        else
                        {
                            raise_error(E_SYNTAX, "Expected datatype after a parameter.");
                            match(token_blank); // @TODO len sa posunut, zaruceny fail sice,..
                        }

                        if (active_token->type == token_comma)
                        {
                            match(token_comma);
                            if (param_list_append(&function_parameters, &new_parameter))
                                continue;
                        }
                        else if (active_token->type == token_rbrace)
                        {
                            param_list_append(&function_parameters, &new_parameter);
                            break;
                        }
                        else
                        {
                            // some unexpected token
                            break;
                        }

                    } // end of while loop

                    // <<<<<<<<<<<<<<<<<<
                    //  END OF PARAMETERS
                    // <<<<<<<<<<<<<<<<<<

                    if (match(token_rbrace) == false)
                        raise_error(E_SYNTAX, "')' was expected.");

                    if (match(token_as) == false)
                        raise_error(E_SYNTAX, "'As' keyword was expected.");

                    
                    bool datatypes_equal = false;

                    // check if the token is one of the datatypes
                    if (is_datatype(active_token->type))
                    {
                        // compare the return datatypes
                        if (active_token->type == function_metadata->type)
                        {
                            datatypes_equal = true;
                            function_return_datatype = active_token->type; // prepare return type for function body
                        }
                        else
                        {
                            raise_error(E_SEM_DEF, "Function Datatype doesn't match with it's declaration.");
                        }
                        match(active_token->type);
                    }
                    else // not datatype token
                    {
                        raise_error(E_SYNTAX, "Expected datatype after a parameter.");
                        definition_error = true;
                        match(token_blank); // @TODO len sa posunut, zaruceny fail sice,..
                    }

                    if (match(token_eol) == false)
                        raise_error(E_SYNTAX, "EOL expected at this point.");

                    // COMPARE PARAMETER LISTS
                    if (param_list_cmp(function_metadata->parameters,  function_parameters) == false )
                    {
                        raise_error(E_SEM_DEF, "Function definition parameters do not match with it's declaration.");
                        definition_error = true;
                    }

                    //INST
                    add_inst("LABEL", i_null, "myFunc", i_null,NULL,i_null,NULL);
                    add_inst("PUSHFRAME", i_null,NULL,i_null,NULL,i_null,NULL);
                    add_inst("DEFVAR", i_lf, "%retval", i_null,NULL,i_null,NULL);
        

                    if (!definition_error)
                    {
                        // -----------------
                        // NEW LOCAL SCOPE
                        // -----------------

                        stl_push(&variables);

                        while (function_parameters != NULL)
                        {
                            Metadata_t var_from_parameter;
                            var_from_parameter.parameters = NULL;
                            var_from_parameter.is_declared = true;
                            var_from_parameter.is_defined = false;
                            var_from_parameter.type = function_parameters->type;

                            stl_insert_top(variables, function_parameters->name, &var_from_parameter);
                            function_parameters = function_parameters->next;
                        }

                        NT_CompoundStmt();

                        stl_pop(&variables);

                        // ------------------
                        // END OF LOCAL SCOPE
                        // ------------------
                    }
                    else
                    {
                        // -----------------
                        // NEW LOCAL SCOPE
                        // -----------------

                        stl_push(&variables);

                        NT_CompoundStmt();

                        stl_pop(&variables);

                        // ------------------
                        // END OF LOCAL SCOPE
                        // ------------------
                    }
                    if (match(token_end) == false)
                        raise_error(E_SYNTAX, "'End' keyword was expected at this point.");

                    if (match(token_function) == false)
                        raise_error(E_SYNTAX, "'Function' keyword expected at this point.");
						  param_list_dispose(function_parameters);

                    //INST
                    add_inst("LABEL", i_end, "myFunc", i_null,NULL,i_null,NULL);
                    add_inst("POPFRAME", i_null,NULL,i_null,NULL,i_null,NULL);
                    add_inst("RETURN", i_null,NULL,i_null,NULL,i_null,NULL);

                    return; // !IMPORTANT

                } // end of 'else if (function_metadata->is_declared)'
            } // end of 'if (function_metadata)' 
            else   // -> THERE IS NO SUCH FUNCTION IN SYMTABLE
            {
                // create new instance
                // create a new symtable item locally.
                new_function_metadata.is_declared = true;
                new_function_metadata.is_defined = true;
                new_function_metadata.parameters = NULL;

                // create copy of the string
                new_function_name = (char *)malloc(sizeof(char) * strlen(active_token->value.c) + 1);
                strcpy(new_function_name, active_token->value.c);

                match(token_identifier);

                if (match(token_lbrace) == false)
                    raise_error(E_SYNTAX, "'(' was expected.");

                // >>>>>>>>>>>>>>>
                //  PARAMETERS
                // >>>>>>>>>>>>>>>

                Parameter_t *function_parameters = NULL;

                while (true)
                {
                    // in case of undefined symbol or no parameters
                    if (active_token->type != token_identifier)
                        break;

                    // create new Parameter
                    Parameter_t new_parameter;
                    new_parameter.name = (char *)malloc(sizeof(char) * strlen(active_token->value.c) + 1);
                    strcpy(new_parameter.name, active_token->value.c);
                    new_parameter.next = NULL;

                    match(token_identifier);

                    if (match(token_as) == false)
                        raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");

                    // check if the token is one of the datatypes
                    if (is_datatype(active_token->type))
                    {
                        new_parameter.type = active_token->type;
                        match(active_token->type);
                    }
                    else
                    {
                        raise_error(E_SYNTAX, "Expected datatype after a parameter.");
                        match(token_blank); // @TODO len sa posunut, zaruceny fail sice,..
                    }

                    if (active_token->type == token_comma)
                    {
                        match(token_comma);
                        if (param_list_append(&function_parameters, &new_parameter))
                            continue;
                    }
                    else if (active_token->type == token_rbrace)
                    {
                        param_list_append(&function_parameters, &new_parameter);
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
                    function_return_datatype = active_token->type; // prepare return type for function body
                    match(active_token->type);
                }
                else
                {
                    raise_error(E_SYNTAX, "Expected datatype after a parameter.");
                    definition_error = true;
                    match(token_blank); // @TODO len sa posunut, zaruceny fail sice,..
                }

                if (match(token_eol) == false)
                    raise_error(E_SYNTAX, "EOL expected at this point.");


                // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                //  GENERATE CODE FROM CompoundStmt
                // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

                    // -----------------
                    // NEW LOCAL SCOPE
                    // -----------------

                    stl_push(&variables);

                    while (function_parameters != NULL)
                    {
                        Metadata_t var_from_parameter;
                        var_from_parameter.parameters = NULL;
                        var_from_parameter.is_declared = true;
                        var_from_parameter.is_defined = false;
                        var_from_parameter.type = function_parameters->type;

                        stl_insert_top(variables, function_parameters->name, &var_from_parameter);
                        function_parameters = function_parameters->next;
                    }

                    NT_CompoundStmt();

                    stl_pop(&variables);

                    // ------------------
                    // END OF LOCAL SCOPE
                    // ------------------

                // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                // END OF CODE GENERATING
                // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

                if (match(token_end) == false)
                    raise_error(E_SYNTAX, "'End' keyword was expected at this point.");

                if (match(token_function) == false)
                    raise_error(E_SYNTAX, "'Function' keyword expected at this point.");


                // FINALLY add to symtable if there are no error during declaration
                if (!definition_error) //&& !compiler_error)
                {
                    stl_insert_top(functions, new_function_name, &new_function_metadata);
					free(new_function_name);
					param_list_dispose(function_parameters);
                    return;
				}
				param_list_dispose(function_parameters);
            }
        } // end of IF identifier
        
        // if identifier was not found, but we dont want to ruin syntax of the whole statement
        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Function identifier expected.");

        if (match(token_lbrace) == false)
            raise_error(E_SYNTAX, "'(' was expected.");

        NT_ParamList();

        if (match(token_rbrace) == false)
            raise_error(E_SYNTAX, "')' was expected.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "'As' keyword was expected.");

        if (is_datatype(active_token->type))
        {
            match(active_token->type);
        }
        else
        {
            raise_error(E_SYNTAX, "Expected datatype after a parameter.");
            match(token_blank); // just move
        }

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        // -----------------
        // NEW LOCAL SCOPE / DO NOT GENERATE CODE / JUST SYNTAX CHECK
        // -----------------

        stl_push(&variables);

            NT_CompoundStmt();

        stl_pop(&variables);

        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        if (match(token_end) == false)
            raise_error(E_SYNTAX, "'End' keyword was expected at this point.");

        if (match(token_function) == false)
            raise_error(E_SYNTAX, "'Function' keyword expected at this point.");

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

        if (is_datatype(active_token->type))
        {
            match(active_token->type);
        }
        else
        {
            raise_error(E_SYNTAX, "Expected datatype after a parameter.");
            match(token_blank); // just move
        }
    }

/*****************************************************************************/

    void NT_Scope()
    {   

        while(active_token->type == token_eol)
        {
            match(token_eol);
        }

        if (match(token_scope) == false) // keyword 'Scope'
            raise_error(E_SYNTAX, "Expected 'Scope' keyword not found.");

        add_inst("LABEL main", i_null, NULL, i_null, NULL, i_null, NULL);

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------
        
        stl_push(&variables);

            NT_CompoundStmt();

        stl_pop(&variables);

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





/*****************************************************************************/
    
    // RETURNS the name of the function being declared/defined

    char * NT_VarDec()
    {
        if (match(token_dim) == false)
            raise_error(E_SYNTAX, "Keyword 'Dim' was expected.");

        
        char *new_variable_name = NULL;

        if (active_token->type == token_identifier)
        {
            Metadata_t *variable_metadata = stl_search(variables, active_token->value.c);

            // if variable already exists
            if (variable_metadata)
            {
                raise_error(E_SEM_DEF, "This variable is already declared.");
            }
            else
            {
                // +++++++++++++++++++++
                //  create new instance
                // ++++++++++++++++++++

                // save the name
                new_variable_name = (char *)malloc(sizeof(char) * strlen(active_token->value.c) + 1);
                strcpy(new_variable_name, active_token->value.c);


                match(token_identifier);

        	//INST
        	printf("\ngenerate\n");
        	add_inst("DEFVAR", i_lf, new_variable_name, i_null,NULL,i_null,NULL);


                if (match(token_as) == false)
                    raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");


                // check if the token is one of the datatypes
                if (is_datatype(active_token->type))
                {
                    // Create metadata for the new Variable
                    Metadata_t new_variable_metadata;

                    new_variable_metadata.parameters = NULL;
                    new_variable_metadata.type = active_token->type;
                    new_variable_metadata.is_defined = 0;
                    new_variable_metadata.is_declared = 1;

                    match(active_token->type);

                    if (stl_insert_top(variables, new_variable_name, &new_variable_metadata))
                    {
                        return new_variable_name;
                    }

                    return NULL;
                }
                else // not datatype token
                {
                    raise_error(E_SYNTAX, "Expected datatype at this point.");
                    match(token_blank); // @TODO len sa posunut, zaruceny fail sice,..
                    return NULL;
                }
            }
        }
        
        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier expected and not found.");

        if (match(token_as) == false)
            raise_error(E_SYNTAX, "Keyword 'As' expected and not found.");

        // check if the token is one of the datatypes
        if (is_datatype(active_token->type))
        {
            match(active_token->type);
        }
        else
        {
            raise_error(E_SYNTAX, "Expected datatype at this point.");
            match(token_blank); // @TODO len sa posunut, zaruceny fail sice,..
        }


        return NULL;
    }


/*****************************************************************************/

    void NT_VarDef()
    {
        // gets the name of declared variable if everything is OK
        char* variable_name = NT_VarDec();
        Metadata_t *var_meta = stl_search(variables, variable_name);

        if (active_token->type == token_op_eq)
        {
            match(token_op_eq);
            
            if (variable_name)
            {
                //INST
                NT_Expr(var_meta->type);
        	      add_inst("POPS", i_lf, variable_name, i_null,NULL,i_null,NULL);
                free(variable_name);
                return;
            }
            else 
            {
                // if there was error declaring the variable or 
                // the variable was already declared
                raise_error(E_SEM_DEF, "Cannot define a variable.");
                //NT_Expr();
            }
        }

        free(variable_name);
        // if no equals, just declare.. 
        // finish generating instructions just for declaration
    }

// /*****************************************************************************/

    void NT_AssignStmt()
    {
        // @TODO >>>>>>>>
        // Generate instruction, declare inside symtable.. etc.

        Metadata_t *variable = NULL;

        if (active_token->type == token_identifier)
        {
            char name[strlen(active_token->value.c) + 1];
            strcpy(name, active_token->value.c);
            variable = stl_search(variables, active_token->value.c);

            if (variable != NULL)
            {
                match(token_identifier);

                if (match(token_op_eq) == false)
                    raise_error(E_SYNTAX, "Assignment operator '=' expected.");
                NT_Expr(variable->type);

		            add_inst("POPS", i_lf, name, i_null,NULL,i_null,NULL);
                //fprintf(output_code, "pops lf@%s\n", name);
                return;
            }
            else
            {
                raise_error(E_SEM_OTHER, "This variable doesn't exist.");
            }
        }

        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier expected and not found.");


        if (match(token_op_eq) == false)
            raise_error(E_SYNTAX, "Assignment operator '=' expected.");
    }

// /*****************************************************************************/

    void NT_IfStmt()
    {

        if (match(token_if) == false)
            raise_error(E_SYNTAX, "Keyword 'If' expected.");


        NT_Expr(token_boolean);
        
        //INST
        next_tmp_name("els");
        char* else_label = malloc(sizeof(char)*16);
        strcpy(else_label, tmp_name);
        next_tmp_name("pop");
        add_inst("POPS", i_gf, tmp_name, i_null,NULL,i_null,NULL);
        add_inst("JUMPIFEQ", i_null, else_label, i_bool, "false", i_gf, tmp_name);

        if (match(token_then) == false)
            raise_error(E_SYNTAX, "Keyword 'Then' expected.");

        
        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------

        stl_push(&variables);

            NT_CompoundStmt();

        stl_pop(&variables);

        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        if (match(token_else) == false)
            raise_error(E_SYNTAX, "Keyword 'Else' expected.");

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected at this point.");

        //INST
        add_inst("JUMP", i_end, else_label, i_null,NULL,i_null,NULL);
        add_inst("LABEL", i_null, else_label, i_null,NULL,i_null,NULL);

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------

        stl_push(&variables);

            NT_CompoundStmt();

        stl_pop(&variables);

        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        if (match(token_end) == false)
            raise_error(E_SYNTAX, "Keyword 'End' expected.");

        if (match(token_if) == false)
            raise_error(E_SYNTAX, "Keyword 'If' expected.");

        add_inst("LABEL", i_end, else_label, i_null,NULL,i_null,NULL);
        free(else_label);
    }


// /*****************************************************************************/

    void NT_WhileStmt()
    {
        if (match(token_do) == false)
            raise_error(E_SYNTAX, "Keyword 'Do' expected.");

        if (match(token_while) == false)
            raise_error(E_SYNTAX, "Keyword 'While' expected.");
       
        //INST
        next_tmp_name("whl");
        char* while_label = malloc(sizeof(char)*16);
        strcpy(while_label, tmp_name);
        add_inst("LABEL", i_null, while_label, i_null,NULL,i_null,NULL);

        NT_Expr(token_boolean);
      
        next_tmp_name("pop");
        add_inst("POPS", i_gf, tmp_name, i_null,NULL,i_null,NULL);
        add_inst("JUMPIFEQ", i_end, while_label, i_bool, "false", i_gf, tmp_name);      

        if (match(token_eol) == false)
            raise_error(E_SYNTAX, "EOL expected.");

        // -----------------
        // NEW LOCAL SCOPE
        // -----------------


        stl_push(&variables);

            NT_CompoundStmt();

        stl_pop(&variables);

        // ------------------
        // END OF LOCAL SCOPE
        // ------------------

        add_inst("JUMP", i_null, while_label, i_null,NULL,i_null,NULL);

        if (match(token_loop) == false)
            raise_error(E_SYNTAX, "Keyword 'Loop' expected");

        add_inst("LABEL", i_end, while_label, i_null,NULL,i_null,NULL);
        free(while_label);

    }


// /*****************************************************************************/

        void NT_ReturnStmt()
        {
            if (match(token_return) == false)
                raise_error(E_SYNTAX, "Keyword 'Return' expected.");

            // function return datatype gets updated each time 
            // new function is added to symtable
            NT_Expr(function_return_datatype);
            // generate instructions
            add_inst("POPS", i_lf, "%retval", i_null,NULL,i_null,NULL);
            add_inst("POPFRAME", i_null,NULL,i_null,NULL,i_null,NULL);
            add_inst("RETURN", i_null,NULL,i_null,NULL,i_null,NULL);
        }


/*****************************************************************************/


    void NT_InputStmt()
    {
        if (match(token_input) == false)
            raise_error(E_SYNTAX, "Keyword 'source_code' expected.");

        if (active_token->type == token_identifier)
        {
            // check if identifier is declared
            Metadata_t *variable_metadata = stl_search(variables, active_token->value.c);
            
            if (variable_metadata && variable_metadata->is_declared)
            {

                add_inst("READ", i_lf, active_token->value.c, i_null, i2type(variable_metadata->type), i_null, NULL);
            }
            else
            {
                raise_error(E_SEM_OTHER, "Variable '",active_token->value.c,"' is not declared.");
            }
        }

        if (match(token_identifier) == false)
            raise_error(E_SYNTAX, "Identifier is expected right after 'source_code' keyword.");
    }

/*****************************************************************************/

    void NT_PrintStmt()
    {
        if (match(token_print) == false)
            raise_error(E_SYNTAX, "Keyword 'Print' expected.");
        
        //NT_Expr();

        if (match(token_semicolon) == false)
            raise_error(E_SYNTAX, "Semicolon ';' is missing. ");

        NT_ExprList();

    }

/*****************************************************************************/

    void NT_ExprList()
    {
        
        while(active_token->type != token_eol)
        {
            //NT_Expr( '??' ,variables);
            
             next_tmp_name("pop"); //creates new variable name in tmp_cnt
             add_inst("DEFVAR", i_gf , tmp_name , i_null,NULL,i_null,NULL);
             add_inst("POPS", i_gf , tmp_name , i_null,NULL,i_null,NULL);
             add_inst("WRITE", i_gf , tmp_name , i_null,NULL,i_null,NULL);

            if (match(token_semicolon) == false)
                raise_error(E_SYNTAX, "Semicolon ';' is missing. ");
        }

        // epsilon rule otherwise
    }


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

//         //INST
//         add_inst("CREATEFRAME", i_null,NULL,i_null,NULL,i_null,NULL);

//         NT_TermList();

//         if (match(token_rbrace) == false)
//             raise_error(E_SYNTAX, "Right brace ')' expected.");

//         //INST
//         add_inst("CALL", i_null, function name , i_null,NULL,i_null,NULL);
        
//     }


// /*****************************************************************************/

/*    void NT_Expr()
    {
       printf("\n**MAGIC**\n");
       match(token_val_integer);
       printf("was here");
    }*/

/**
 * generates new name in tmp_name
 */
void next_tmp_name(char *spec)
{
  sprintf(tmp_name, "%s$%d", spec, tmp_cnt);
  tmp_cnt++;
}
