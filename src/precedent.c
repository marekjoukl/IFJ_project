#include "precedent.h"
#include "scanner.h"

valid_itmes_t convert_lex_term(Lexeme lex)
{
    valid_itmes_t item;
    switch (lex.kind)
    {
        case LESS:                  item.type = LESS_T; break;
        case LESS_EQUAL:            item.type = LESS_EQUAL_T; break;
        case GREATER:               item.type = GREATER_T; break;
        case GREATER_EQUAL:         item.type = GREATER_EQUAL_T; break;
        case EXCLAMATION_POINT:     item.type = EXCLAMATION_POINT_T; break;
        case NOT_EQUAL:             item.type = NOT_EQUAL_T; break;
        case EQUAL:                 item.type = EQUAL_T; break;
        case RIGHT_PAR:             item.type = RIGHT_PAR_T; break;
        case LEFT_PAR:              item.type = LEFT_PAR_T; break;
        case DOUBLE_QUESTION_MARK:  item.type = DOUBLE_QUESTION_MARK_T; break;
        case PLUS:                  item.type = PLUS_T; break;
        case MINUS:                 item.type = MINUS_T; break;
        case ASTERISK:              item.type = MUL_T; break;
        case SLASH:                 item.type = DIV_T; break;
        case IDENTIFIER:            item.type = TERM_T;
                                    item.var_type = TYPE_UNDEFINED;
                                    break;
        case STRING_LIT:            item.type = TERM_T; 
                                    item.var_type = TYPE_STRING;
                                    break;
        case INTEGER_LIT:           item.type = TERM_T; 
                                    item.var_type = TYPE_INT;
                                    break;
        case DOUBLE_LIT:            item.type = TERM_T;
                                    item.var_type = TYPE_DOUBLE;
                                    break;
        case NIL:                   item.type = TERM_T; 
                                    item.var_type = TYPE_NIL;
                                    break;
        default:                    item.type = DOLLAR_T; break;
    }
    return item;
}

void modify_terminal(valid_itmes_t *item, Lexeme lex, symtable_stack_t *sym_stack)
{
    symtable_item_t *variable = NULL;
    item->posfix_name = NULL;
    item->tree = NULL;

    if(item->type == TERM_T)
    {
        switch (item->var_type)
        {
        case TYPE_UNDEFINED:
            variable = SymtableSearchAll(sym_stack, lex.extra_data.string);
            if(variable == NULL){
                Lexeme *token = &lex;
                ERROR_HANDLE_PREC(UNDEFINED_VAR_ERROR,token);
            }

            item->var_type = variable->data->item_type;
            item->can_be_nil = variable->data->can_be_nil;
            item->is_lit = false;
            item->posfix_name = malloc(sizeof(char) * strlen(lex.extra_data.string) + 1);
            strcpy(item->posfix_name, lex.extra_data.string);
            break;
        
        case TYPE_NIL:
            item->is_lit = true;
            item->can_be_nil = true;
            item->posfix_name = malloc(sizeof(char) * (4));
            strcpy(item->posfix_name, "nil");
            break;;

        default:
            item->can_be_nil = false;
            item->is_lit = true;
            item->posfix_name = malloc(sizeof(char) * (strlen(lex.extra_data.string) + 1));
            strcpy(item->posfix_name, lex.extra_data.string);
            break;
        }               
    }
}

const stack_rules_t prec_table[TERMINAL_CNT_T][TERMINAL_CNT_T] = 
{ // +           -           *           /           ==          !=          <           >           <=          >=          ??          !           (           )         term        $
    {MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // +
    {MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // -
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // *
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // /
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // ==
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // !=
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // <  
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // >
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // <=
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // >=
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // ??
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , ERROR_R   , MERGE_R , ERROR_R   , MERGE_R}, // !
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, SHIFT_R , STOPPAGE_R, ERROR_R}, // (
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , MERGE_R , ERROR_R   , MERGE_R}, // )
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , MERGE_R , ERROR_R   , MERGE_R}, // term
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R , STOPPAGE_R, ERROR_R}  // $   
};


stack_rules_t give_stack_rule(prec_stack_t *stack, prec_terminal_t input)
{
    valid_itmes_t top;
    stack_top_terminal(stack, &top);
    return prec_table[top.type][input];
}

bool check_prec_rule(prec_stack_t *stack, valid_itmes_t *new_expression, Lexeme *token)
{
    // printf("\n ====NEW===== \ntree1: \n");
    //     tree_postorder(*asttree1);
    //     puts("\ntree2:");
    //     tree_postorder(*asttree2);
    //     puts("");


    valid_itmes_t rule;
    
    ast_t *new_parent = NULL;
    ast_t *new_left = NULL;
    ast_t *new_right = NULL;

    stack_top_terminal(stack, &rule);
    bool valid = false;
    new_expression->type = EXPRESSION_T;
    new_expression->posfix_name = NULL;

    switch (rule.type)
    {
    case MUL_T:
        valid = rule3(stack, rule);

        if((stack->items.var_type != TYPE_INT && stack->items.var_type != TYPE_DOUBLE) || 
            (stack->next->next->items.var_type != TYPE_INT && stack->next->next->items.var_type != TYPE_DOUBLE))
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->var_type = stack->items.var_type;
        new_expression->can_be_nil = false;


        printf("right: %s:%d\n", stack->items.posfix_name, stack->items.is_lit); //debug
        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;
        
        printf("left: %s:%d\n", stack->next->next->items.posfix_name, stack->next->next->items.is_lit); //debug
        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;


        // Int2Double
        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_left, "i2d");
            }

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_right, "i2d");
            }

            new_expression->var_type = TYPE_DOUBLE;
        }  

        if(stack->items.is_lit == false)
            new_expression->is_lit = stack->items.is_lit;
        else
            new_expression->is_lit = stack->next->next->items.is_lit;
        
        tree_insert(&new_parent, "*");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;

        break;
    
/*=========================================================================================================*/    
    case DIV_T:
        valid = rule3(stack, rule);    
        if((stack->items.var_type != TYPE_INT && stack->items.var_type != TYPE_DOUBLE) || 
            (stack->next->next->items.var_type != TYPE_INT && stack->next->next->items.var_type != TYPE_DOUBLE))
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if (stack->items.var_type != stack->next->next->items.var_type)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = stack->items.var_type;

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;

        if(stack->items.is_lit == false)
            new_expression->is_lit = stack->items.is_lit;
        else
            new_expression->is_lit = stack->next->next->items.is_lit;

        tree_insert(&new_parent, "/");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;

        break;
/*=========================================================================================================*/
    case PLUS_T:
        valid = rule3(stack, rule);

        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED ||
           stack->next->next->items.var_type == TYPE_BOOL || stack->next->next->items.var_type == TYPE_UNDEFINED)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = stack->items.var_type;

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;

        // Int2Double
        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                if(new_left != NULL)
                    tree_insert(&new_left, "i2d");
            }

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_right, "i2d");
            }

            new_expression->var_type = TYPE_DOUBLE;
        }
        
        if(stack->items.is_lit == false)
            new_expression->is_lit = stack->items.is_lit;
        else
            new_expression->is_lit = stack->next->next->items.is_lit;

        tree_insert(&new_parent, "+");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;
        break;
    
/*=========================================================================================================*/    
    case MINUS_T:
        valid = rule3(stack, rule);

        if((stack->items.var_type != TYPE_INT && stack->items.var_type != TYPE_DOUBLE) || 
            (stack->next->next->items.var_type != TYPE_INT && stack->next->next->items.var_type != TYPE_DOUBLE))
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = stack->items.var_type;

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;

        // Int2Double
        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_left, "i2d");
            }
            
            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_right, "i2d");
            }

            new_expression->var_type = TYPE_DOUBLE;
        }

        if(stack->items.is_lit == false)
            new_expression->is_lit = stack->items.is_lit;
        else
            new_expression->is_lit = stack->next->next->items.is_lit;

        tree_insert(&new_parent, "-");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;
        break;
    
/*=========================================================================================================*/    
    case EQUAL_T:
        valid = rule3(stack, rule);

        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;

        // Int2Double
        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_left, "i2d");
            }
            
            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_right, "i2d");
            }
        }

        tree_insert(&new_parent, "==");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;

        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;
    
/*=========================================================================================================*/    
    case NOT_EQUAL_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;

        // Int2Double
        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_left, "i2d");
            }
            
            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                tree_insert(&new_right, "i2d");
            }
        }

        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        tree_insert(&new_parent, "!=");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;
        break;
    
/*=========================================================================================================*/    
    case LESS_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED || stack->items.var_type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            // if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
            //     {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            // if(stack->items.var_type == TYPE_INT){
            //     if(stack->items.is_lit == false)
            //         {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            //     // int2double TODO
            // }
            
            // if(stack->next->next->items.var_type == TYPE_INT){
            //     if(stack->next->next->items.is_lit == false)
            //         {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            //     // int2double TODO
            // }
            ERROR_HANDLE_PREC(TYPE_ERROR, token);
        }

        if(stack->items.can_be_nil == true || stack->next->next->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;
        tree_insert(&new_parent, "<");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;
        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;

/*=========================================================================================================*/    
    case GREATER_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED || stack->items.var_type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            
        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            // if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            // if(stack->items.var_type == TYPE_INT){
            //     if(stack->items.is_lit == false)
            //         {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            //     // int2double TODO
            // }
            
            // if(stack->next->next->items.var_type == TYPE_INT){
            //     if(stack->next->next->items.is_lit == false)
            //         {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            //     // int2double TODO
            // }
        }

        if(stack->items.can_be_nil == true || stack->next->next->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;
        tree_insert(&new_parent, ">");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;
        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;

/*=========================================================================================================*/    
    case LESS_EQUAL_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED || stack->items.var_type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            // if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            // if(stack->items.var_type == TYPE_INT){
            //     if(stack->items.is_lit == false)
            //         {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            //     // int2double TODO
            // }
            
            // if(stack->next->next->items.var_type == TYPE_INT){
            //     if(stack->next->next->items.is_lit == false)
            //         {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            //     // int2double TODO
            // }
        }

        if(stack->items.can_be_nil == true || stack->next->next->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;
        tree_insert(&new_parent, "<=");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;
        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;

/*=========================================================================================================*/    
    case GREATER_EQUAL_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED || stack->items.var_type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            // if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            // if(stack->items.var_type == TYPE_INT){
            //     if(stack->items.is_lit == false)
            //         {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            //     // int2double TODO
            // }
            
            // if(stack->next->next->items.var_type == TYPE_INT){
            //     if(stack->next->next->items.is_lit == false)
            //         {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            //     // int2double TODO
            // }
        }

        if(stack->items.can_be_nil == true || stack->next->next->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;
        tree_insert(&new_parent, ">=");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;
        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;
    
/*=========================================================================================================*/    
    case DOUBLE_QUESTION_MARK_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type != stack->next->next->items.var_type && stack->next->next->items.type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.posfix_name != NULL)
            tree_insert(&new_right, stack->items.posfix_name);
        else
            new_right = stack->items.tree;

        if(stack->next->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->next->items.posfix_name);
        else
            new_left = stack->next->next->items.tree;
        tree_insert(&new_parent, "??");
        tree_link(&new_parent, new_left, new_right);
        new_expression->tree = new_parent;
        new_expression->can_be_nil = false;
        new_expression->var_type = stack->items.var_type;
        break;
    
/*=========================================================================================================*/    
    case EXCLAMATION_POINT_T:
        valid = (check_stoppage(stack, 2) && stack->items.type == rule.type && stack->next->items.type == EXPRESSION_T);
        new_expression->can_be_nil = false;
        new_expression->var_type = stack->next->items.var_type;
        new_expression->is_lit = stack->next->items.is_lit;
        if(stack->next->items.posfix_name != NULL)
            tree_insert(&new_left, stack->next->items.posfix_name);
        else
            new_left = stack->next->items.tree;
        tree_insert(&new_parent, "!");
        tree_link(&new_parent, new_left, NULL);
        new_expression->tree = new_parent;
        break;
    
/*=========================================================================================================*/    
    case RIGHT_PAR_T:
        valid = (check_stoppage(stack,3) && stack->items.type == RIGHT_PAR_T && stack->next->items.type == EXPRESSION_T && stack->next->next->items.type == LEFT_PAR_T);
        new_expression->can_be_nil = stack->next->items.can_be_nil;
        new_expression->var_type = stack->next->items.var_type;
        new_expression->is_lit = stack->next->items.is_lit;
        break;
    
/*=========================================================================================================*/    
    case TERM_T:
        valid = rule1(stack, rule);
        new_expression->can_be_nil = stack->items.can_be_nil;
        new_expression->var_type = stack->items.var_type;
        new_expression->is_lit = stack->items.is_lit;
        // puts("\nTERM"); //debug
        new_expression->posfix_name = stack->items.posfix_name;
        // printf("new_term = %s\n", new_expression->posfix_name); //debug
        break;  
    
    default:
        ERROR_HANDLE_PREC(SYNTAX_ERROR, token);
        break;
    }

    return valid;
}

data_type_t precedent_analysys(Lexeme *lexeme, symtable_stack_t *sym_stack, ast_t **asttree)
{
    bool valid = true;
    bool cont = true;
    prec_stack_t *stack;
    stack_init(&stack);
    stack_rules_t stack_rule;
    symtable_item_t *variable;
    valid_itmes_t new_expression;

    if(lexeme->kind == IDENTIFIER)
    {
        variable = SymtableSearchAll(sym_stack, lexeme->extra_data.string);
        if(variable == NULL)
            {ERROR_HANDLE_PREC(UNDEFINED_VAR_ERROR, lexeme);}
    }

    valid_itmes_t input = convert_lex_term(*lexeme);
    
    while(valid)
    {
        //check if variable was defined
        // if(lexeme->kind == IDENTIFIER)
        // {
        //     variable = SymtableSearchAll(sym_stack, lexeme->extra_data.string);
        //     if(variable == NULL)
        //         {ERROR_HANDLE_PREC(UNDEFINED_VAR_ERROR, lexeme);}
        // }

        stack_rule = give_stack_rule(stack, input.type);

        switch (stack_rule)
        {
        case SHIFT_R:
            modify_terminal(&input, *lexeme, sym_stack);
            stack_push(&stack, &input);
            *lexeme = get_next_non_whitespace_lexeme();
            input = convert_lex_term(*lexeme);
            break;
        case STOPPAGE_R:
            modify_terminal(&input, *lexeme, sym_stack);
            stack_push_stoppage(&stack);
            stack_push(&stack, &input);
            *lexeme = get_next_non_whitespace_lexeme();
            input = convert_lex_term(*lexeme);
            break;
        case MERGE_R:
            if(check_prec_rule(stack, &new_expression, lexeme)){
                // printf("new_e:%s\n\n\n", new_expression.posfix_name); //debug
                stack_merge(&stack, new_expression);}
            else
                {ERROR_HANDLE_PREC(SYNTAX_ERROR, lexeme);}
            break;
        case ERROR_R:
            valid = false;
            break;
        default:

            ERROR_HANDLE_PREC(SYNTAX_ERROR, lexeme);
            break;
        }
    }

    // second traverse with input = $
    input.type = DOLLAR_T;
    while(cont == true)
    {
        stack_rule = give_stack_rule(stack, input.type);
        if(stack_rule == MERGE_R){
            if(check_prec_rule(stack, &new_expression, lexeme))
                stack_merge(&stack, new_expression);
            else
                {ERROR_HANDLE_PREC(SYNTAX_ERROR, lexeme);}
        }
        else
            cont = false;
    }
    
    // check if all syntax operations valid
        if(!stack_empty(stack))
        {ERROR_HANDLE_PREC(SYNTAX_ERROR, lexeme);}

    // puts("");
    // tree_postorder(stack->items.tree);
    // puts("\n");
    
    *asttree = stack->items.tree;
    data_type_t exit_data_type = stack->items.var_type;
    stack_dispose(&stack); 

    return exit_data_type;
}
