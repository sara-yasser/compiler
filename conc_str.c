#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add_to_code(char** code, int *code_index, char* s){
    int len = strlen(s);
    code[*code_index] = malloc((len + 1) * sizeof(char));
    strcpy(code[*code_index], s);
    *code_index += 1;
}

void disply_code(char** code, int code_index){
    for(int i=0;i<code_index;i++){
        printf("token %s\n", code[i]);
    }
}

enum EXPR_TYPE {
    EXPR = 0,
    START_IF_EXPR,
    START_ELSE_IF_EXPR,
    END_IF_EXPR,
    END_COND_EXPR,
    PROC,
    RTN,
    RTN_SMTH,
};

enum OP {
    ADD = 0,
    SUB,
    MUL,
    DIV,
    MOD,
    OR,
    AND,
    EQ,
    NEQ,
    GT,
    LT,
    GTE,
    LTE,
};

struct Proc {
  char* func_name;
  char* ret_type;
};

struct Expr {
    char* identifier;
    char** args;
    enum OP* ops;
    int ops_len;
    int args_len;
};

struct StartIfExpr {};
struct StartElseIfExpr {};
struct EndIfExpr {};
struct EndCondExpr {};
struct Rtn {};
struct RtnSmth {};

void clear_arr(char **arr, int size);
void compile(char* assembly, void **all_expr, enum EXPR_TYPE* all_expr_type, int expr_idx);
char** clone_args(char** arr, int size);
char** remove_from_arg(int pos, int *size, char** arr, char* reg);
void remove_from_op(int pos, int *size, enum OP* arr);
void print_arr_string(char** arr, int size);
void print_op_arr(enum EXPR_TYPE* arr, int size);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void add_to_allexpr(void **all_expr, enum EXPR_TYPE* all_expr_type, int *expr_idx, void* expr, enum EXPR_TYPE expr_type) {
    all_expr_type[*expr_idx] = expr_type;
    switch (expr_type) {
        case EXPR:
            all_expr[(*expr_idx)++] = (struct Expr*)expr;
            break;
        case START_IF_EXPR:
            all_expr[(*expr_idx)++] = (struct StartIfExpr*)expr;
            break;
        case START_ELSE_IF_EXPR:
            all_expr[(*expr_idx)++] = (struct StartElseIfExpr*)expr;
            break;
        case END_IF_EXPR:
            all_expr[(*expr_idx)++] = (struct EndIfExpr*)expr;
            break;
        case END_COND_EXPR:
            all_expr[(*expr_idx)++] = (struct EndCondExpr*)expr;
            break;
        case PROC:
            all_expr[(*expr_idx)++] = (struct Proc*)expr;
            break;
        case RTN:
            all_expr[(*expr_idx)++] = (struct Rtn*)expr;
            break;
        case RTN_SMTH:
            all_expr[(*expr_idx)++] = (struct RtnSmth*)expr;
            break;
        default:
            break;
    }
}

void remove_from_op(int pos, int *size, enum OP* arr){
  	for(int i=pos; i<(*size)-1; i++)
    {
        arr[i] = arr[i + 1];
    }
    (*size)--;
}

void clear_arr(char* arr[], int size) {
    for(int i = 0; i < size; i++) {
        free(arr[i]);
    }
    free(arr);
}

char** remove_from_arg(int pos, int *size, char** arr, char* reg){
  	char **new_arr = malloc(((*size)-1) * sizeof(char *));
    for(int i=0; i<pos; i++)
    {
        new_arr[i] = malloc((strlen(arr[i])+1) * sizeof(char));
        strcpy(new_arr[i], arr[i]);
    }
    
  	new_arr[pos] = malloc((strlen(reg)+1) * sizeof(char));
    strcpy(new_arr[pos], reg);

  	for(int i=pos+1; i<(*size)-1; i++)
    {
        new_arr[i] = malloc((strlen(arr[i+1])+1) * sizeof(char));
        strcpy(new_arr[i], arr[i + 1]);
    }
  	clear_arr(arr, *size);
  	(*size)--;
  	return new_arr;
}

char** clone_args(char** arr, int size) {
  	char** new_arr = malloc((size-1) * sizeof(char *));
    for(int i=0; i<size; i++)
    {
        new_arr[i] = malloc((strlen(arr[i])+1) * sizeof(char));
        strcpy(new_arr[i], arr[i]);
    }
    return new_arr;
}

void print_arr_string(char** arr, int size) {
  printf("Debug arr:\n");
  for (int i = 0; i < size; i++) {
    printf("%s\n", arr[i]);
  }
}

void print_op_arr(enum EXPR_TYPE* arr, int size) {
  printf("#################\n");
  printf("Operations:\n");
  for (int i = 0; i < size; i++)
  {
    switch (arr[i])
    {
    case ADD:
      printf("ADD\n");
      break;
    case SUB:
      printf("SUB\n");
      break;
    case MUL:
      printf("MUL\n");
      break;
    case DIV:
      printf("DIV\n");
      break;
    case MOD:
      printf("MOD\n");
      break;
    case OR:
      printf("OR\n");
      break;
    case AND:
      printf("AND\n");
      break;
    case EQ:
      printf("EQ\n");
      break;
    case NEQ:
      printf("NEQ\n");
      break;
    case GT:
      printf("GT\n");
      break;
    case GTE:
      printf("GTE\n");
      break;
    case LT:
      printf("LT\n");
      break;
    case LTE:
      printf("LTE\n");
      break;
    default:
      break;
    }
  }
  printf("#################\n");
  
}

void compile(char* assembly, void **all_expr, enum EXPR_TYPE* all_expr_type, int expr_idx) {
    int nextLabelCount = 0;
    int labelEndCount = 0;
    char lastRegUsed[20] = "";
    for (int ii = 0; ii < expr_idx; ++ii) {
        char **new_args;
        char ifEndLabel[20] = "LabelCondEnd";
        char label[50];
        struct Expr* e = (struct Expr *)all_expr[ii];
        struct Proc* p = (struct Proc *)all_expr[ii];
        switch (all_expr_type[ii]) {
            case EXPR:
                // int a = 5 + 3 / 7 - 4 * 5;
                /*
                DIV 3 7 $0
                MUL 3 5 $1
                ADD 5 $0 $2
                SUB $2 $1 $3
                CPY $3 a
                */
                /*
                ops = [+, /, -, *]
                args = [5, 3, 7, 4, 5]
                args2 = [5, $0, 4, 5]
                args3 = [5, 0$, 1$]
                args4 = [2$, 1$]
                args5 = [3$]
                */
                new_args = clone_args(e->args, e->args_len);
                int new_args_len = e->args_len;
                int reg_count = 0;
                int i = 0;
                print_arr_string(e->args, e->args_len);
                print_op_arr(e->ops, e->ops_len);
                while (e->ops_len > 0 && i < e->ops_len) {
                  	if (e->ops[i] == DIV) {
                        strcat(assembly, "DIV ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                  	else if (e->ops[i] == MUL) {
                        strcat(assembly, "MUL ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                  	else if (e->ops[i] == MOD) {
                        strcat(assembly, "MOD ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                    else {
                        i++;
                    }
                }
                i = 0;
                while (e->ops_len > 0 && i < e->ops_len) {
                  	if (e->ops[i] == ADD) {
                        strcat(assembly, "ADD ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                  	else if (e->ops[i] == SUB) {
                        strcat(assembly, "SUB ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                    else {
                    	i++;
                    }
                }
                i = 0;
                while (e->ops_len > 0 && i < e->ops_len) {
                  	if (e->ops[i] == OR) {
                        strcat(assembly, "OR ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                  	else if (e->ops[i] == AND) {
                        strcat(assembly, "AND ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                    else {
                    	i++;
                    }
                }
                i = 0;
                while (e->ops_len > 0 && i < e->ops_len) {
                  	if (e->ops[i] == EQ) {
                        printf("HERE\n");
                        strcat(assembly, "EQ ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                    else if (e->ops[i] == NEQ) {
                        strcat(assembly, "NEQ ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                  	else if (e->ops[i] == GT) {
                        strcat(assembly, "GT ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                  	if (e->ops[i] == GTE) {
                        strcat(assembly, "GTE ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                  	else if (e->ops[i] == LT) {
                        strcat(assembly, "LT ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                  	if (e->ops[i] == LTE) {
                        strcat(assembly, "LTE ");
                        strcat(assembly, new_args[i]);
                        strcat(assembly, " ");
                        strcat(assembly, new_args[i+1]);
                        strcat(assembly, " ");
                        char snum[20];
                        sprintf(snum, "$%d", reg_count++);
                        strcat(assembly, snum);
                        strcpy(lastRegUsed, snum);
                        strcat(assembly, "\n");
                      	remove_from_op(i, &e->ops_len, e->ops);
                      	new_args = remove_from_arg(i, &new_args_len, new_args, snum);
                    }
                    else {
                    	i++;
                    }
                }
                strcat(assembly, "CPY ");
                strcat(assembly, new_args[0]);
                strcat(assembly, " ");
                strcat(assembly, e->identifier);
                strcat(assembly, "\n");

                break;

            case START_IF_EXPR:
                sprintf(label, "JZ %s Label%d\n", lastRegUsed, nextLabelCount);
                strcat(assembly, label);
                break;
            case START_ELSE_IF_EXPR:
                sprintf(label, "JZ %s Label%d\n", lastRegUsed, nextLabelCount);
                strcat(assembly, label);
                break;
            case END_IF_EXPR:
                sprintf(label, "JMP LabelCondEnd%d\n", labelEndCount);
                strcat(assembly, label);
                sprintf(label, "Label%d:\n", nextLabelCount++);
                strcat(assembly, label);
                break;
            case END_COND_EXPR:
                sprintf(label, "LabelCondEnd%d:\n", labelEndCount++);
                strcat(assembly, label);
                break;
            case PROC:
                sprintf(label, "\nPROC %s\n", p->func_name);
                strcat(assembly, label);
                break;
            case RTN:
                sprintf(label, "RTN\n");
                strcat(assembly, label);
                break;
            case RTN_SMTH:
                sprintf(label, "RTN %s\n", lastRegUsed);
                strcat(assembly, label);
                break;
            default:
                break;
        }
    }
}
