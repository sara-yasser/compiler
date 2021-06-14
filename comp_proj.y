%{
        #include <stdio.h>
        #include <string.h>
        #include"symbolTable.c"
        #include"conc_str.c"
        void yyerror(char *);
        void inc_level();
        void dec_level();
        void inc_scope_id();
        int check_compatible(const char* ,const char * );
        int check_math_operations(const char *,const char * );
        #define YYDEBUG 1
        int yylex(void);
        symb_t symbols[1000];
          FILE* fw;
          FILE* ass;
        // char* code[1000];
        // int code_index = 0;
        extern int yylineno;
        int level=0,scope_id=0;
        int table_index=0;
        void print_string(const char* s);
        void print_value(char* s);
        void clear_args(int *args_index, int *op_index){
                *args_index = 0;
                *op_index = 0;
        }
        void add_op(enum OP* ops, int *op_index, enum OP o){
                ops[*op_index] = o;
                (*op_index)++;
        }
        void add_arg(char** args, int *args_index, char* a){
                args[*args_index] = malloc((strlen(a)+1) * sizeof(char));
                strcpy(args[*args_index],a);
                (*args_index)++;
        }

        struct Expr* get_expr(char** args, enum OP* ops, int args_index, int op_index, char* id_e) {
			struct Expr *e = (struct Expr*)malloc(sizeof(struct Expr));
                e->identifier = malloc((strlen(id_e)+1) * sizeof(char));
                strcpy(e->identifier , id_e);
                e->ops_len = op_index;
                e->args_len = args_index;
                e->args = malloc(args_index * sizeof(char*));
                e->ops = malloc(op_index * sizeof(enum OP));
                for (int i = 0; i < args_index; ++i) {
                e->args[i] = malloc((strlen(args[i])+1) * sizeof(char));
                strcpy(e->args[i], args[i]);
            }
            for (int i = 0; i < op_index; ++i) {
      			e->ops[i] = ops[i];
            }
    		return e;
        }
        

        enum EXPR_TYPE type;
        void* all_expr[1000];
        enum EXPR_TYPE all_expr_type[1000];
        int expr_idx = 0;
        //struct IfExpr* if_expr_struct;
        char id_e[20];
        char value_e[20];

        char* args[100];
        enum OP ops[100];
        int args_index = 0;
        int op_index = 0;
        // int e = 5 ==> cpy 5 e

%}

%union {int INTGR; const char * STRNG; float FLT; char CHR;}
%start expressions

%token <STRNG> INTEGER
%token <STRNG> Letters
%token <STRNG> FLOATS
%token <STRNG> If Else FOR While Do For Switch Case Break Default Continue
%token <STRNG> Int Float Char String Bool Void
%token <STRNG> Increment Decrement Comma Constant Return Print
%token <STRNG> Semicolon Colon Right_Bracet Left_Bracet Left_par Right_par Single_quotes 
%token <STRNG> Biggerthan Smallerthan Eq Noteq Biggerthanequal Smallerthanequal And Or
%token <STRNG> Ass_operator
%token <STRNG> True False


%left '+' '-' '*' '/' '%' 
%type <STRNG> parameters_type;
%type <STRNG> diff_expressions;
%type <STRNG> var_decl ;


%type <STRNG> fnc_call;
%type <STRNG> operations_expressions;
%type <STRNG> binary_expr;
%type <STRNG> for_exp;
%type <STRNG> for_paranthesis;
%type <STRNG> or_expr;
%type <STRNG> and_expr;
%type <STRNG> equality_expr;
%type <STRNG> relational_expr;


%%
expressions:
        expressions Right_Bracet expression  Left_Bracet
        |expressions expression  
        |expression  
        
expression:
        expr Semicolon                  
        |conditional_exp                
        |func             
        |diff_expressions Semicolon     
        |Break Semicolon                
        ;

expr:
        var_decl                        //{printf("code = %s\n", $1);}
        |const_decl
        |fnc_call   
        |var_assignment 
        |return_st
        |print_st
        ;                               
var_decl:
         parameters_type Letters        {
                                        strcpy(id_e , $2);
                                        $$=$2;declare($2,*$1,symbols,table_index,yylineno,level,scope_id,fw);table_index++;
                                        // id_e = $2;
                                        }

parameters_type:
         Int                      {$$="int";}            
         |Float                   {$$="float";}   
         |Char                    {$$="char";}   
         |String                  {$$="string";}     
         |Bool                    {$$="boolean";}
         |Void                    {$$="void";}   
if_cond:
        if_exp else_exp 
        |if_exp
conditional_exp :
        // if_exp  {
        //         type = END_IF_EXPR;
        //         struct EndIfExpr *e = malloc(sizeof(struct EndIfExpr));
        //         add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
        //         }
        // |else_exp 
        if_cond         {
                        clear_args(&args_index, &op_index);
                        type = END_COND_EXPR;
                        struct EndCondExpr *e = malloc(sizeof(struct EndCondExpr));
                        add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                        }
        |while_exp
        |do_while_exp
        |switch_exp
        |for_exp
        

func: 
      fnc_header Semicolon
      |fnc_header fnc_body 
fnc_header : 
         parameters_type Letters Right_par parameters Left_par {
                clear_args(&args_index, &op_index);
                type = PROC;
                struct Proc *e = malloc(sizeof(struct Proc));
                e->ret_type = malloc((strlen($1)+1) *sizeof(char));
                e->func_name = malloc((strlen($2)+1) *sizeof(char));
                strcpy(e->ret_type, $1);
                strcpy(e->func_name, $2);
                add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);
                fnc_implementation($2,*$1,symbols,table_index,yylineno,level,scope_id,fw) ;table_index++;
         }
         | parameters_type Letters Right_par Left_par  {
                clear_args(&args_index, &op_index);
                type = PROC;
                struct Proc *e = malloc(sizeof(struct Proc));
                e->ret_type = malloc((strlen($1)+1) *sizeof(char));
                e->func_name = malloc((strlen($2)+1) *sizeof(char));
                strcpy(e->ret_type, $1);
                strcpy(e->func_name, $2);
                add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);
                char* s= strdup($1);
                fnc_implementation($2,*$1,symbols,table_index,yylineno,level,scope_id,fw) ;table_index++;
         }

parameters:
         parameters_type Letters              { declaring_and_ass_parameters($2,*$1,symbols,table_index,yylineno,level,scope_id,fw);
                                                                   table_index ++; }                    
         | parameters Comma parameters_type Letters                {declaring_and_ass_parameters($4,*$3,symbols,table_index,yylineno,level,scope_id,fw);
                                                                   table_index ++; }

fnc_body:
         Right_Bracet Left_Bracet
        |Right_Bracet expressions Left_Bracet 


const_decl:
         Constant parameters_type Letters  Ass_operator diff_expressions      {
                                                                                type = EXPR;
                                                                                strcpy(id_e , $3);
                                                                                struct Expr *e = get_expr(args, ops, args_index, op_index, id_e);
                                                                                add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                                                                                clear_args(&args_index, &op_index);
                                                                                declare_initalize_const($3,*$2,symbols,table_index,yylineno,level,scope_id,fw);table_index ++;

                                                                              }

fnc_call:
         Letters Right_par parameters_in_fnc_call Left_par         {$$=fnc_call($1,symbols,table_index,yylineno,fw);}
         |Letters Right_par Left_par                              {$$=fnc_call($1,symbols,table_index,yylineno,fw);}
parameters_in_fnc_call:
        diff_expressions                                               
         |parameters_in_fnc_call Comma diff_expressions                  

var_assignment: 
        var_decl Ass_operator diff_expressions                         {
                                                                        type = EXPR;
                                                                        struct Expr *e = get_expr(args, ops, args_index, op_index, id_e);
                                                                        add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                                                                        clear_args(&args_index, &op_index);
                                                                         if(check_compatible($1,$3)==1)
                                                                   var_assign($1,symbols,table_index,yylineno,level,scope_id,fw);
                                                                        }
        |Letters Ass_operator diff_expressions                          {
                                                                        type = EXPR;
                                                                        strcpy(id_e , $2);
                                                                        struct Expr *e = get_expr(args, ops, args_index, op_index, id_e);
                                                                        add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                                                                        clear_args(&args_index, &op_index);
                                                                        if(check_compatible($1,$3)==1)
                                                                   var_assign($1,symbols,table_index,yylineno,level,scope_id,fw);
                                                                        }

return_st:
        Return {
                type = RTN;
                struct Rtn *e = malloc(sizeof(struct Rtn));
                add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);
        }
        |Return diff_expressions {
                type = RTN_SMTH;
                struct RtnSmth *e = malloc(sizeof(struct RtnSmth));
                add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);
        }
print_st:
        Print diff_expressions

if_exp:
       start_if_exp Right_Bracet expressions Left_Bracet {
                                                        type = END_IF_EXPR;
                                                        struct EndIfExpr *e = malloc(sizeof(struct EndIfExpr));
                                                        add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                                                        }
       |start_if_exp expression                         {
                                                        type = END_IF_EXPR;
                                                        struct EndIfExpr *e = malloc(sizeof(struct EndIfExpr));
                                                        add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                                                        }

start_if_exp:
        If Right_par diff_expressions Left_par {
                                                type = START_IF_EXPR;
                                                struct StartIfExpr *e = malloc(sizeof(struct StartIfExpr));
                                                add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                                                }
else_exp:
        Else expression
        |Else Right_Bracet expressions Left_Bracet 
        |start_else_if_exp Right_Bracet expressions Left_Bracet {
                                                                type = END_IF_EXPR;
                                                                struct EndIfExpr *e = malloc(sizeof(struct EndIfExpr));
                                                                add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                                                                }

start_else_if_exp:
        Else If Right_par diff_expressions Left_par     {
                                                        type = START_ELSE_IF_EXPR;
                                                        struct StartElseIfExpr *e = malloc(sizeof(struct StartElseIfExpr));
                                                        add_to_allexpr(all_expr, all_expr_type, &expr_idx, e, type);  
                                                        }
/*
if_else_exp:
       If Right_par diff_expressions Left_par Right_Bracet expressions Left_Bracet Else Right_Bracet expressions Left_Bracet 
       |If Right_par diff_expressions Left_par  expression Else  expression
       |If Right_par diff_expressions Left_par  expression Else Right_Bracet expressions Left_Bracet 
       */

while_exp:
        While Right_par diff_expressions Left_par Right_Bracet expressions Left_Bracet 
        |While Right_par diff_expressions Left_par   expression 

do_while_exp:
        Do Right_Bracet expressions Left_Bracet  While Right_par diff_expressions Left_par 
        |Do expression While Right_par diff_expressions Left_par 
        
switch_exp:
        Switch Right_par diff_expressions Left_par  Right_Bracet cases Left_Bracet 


cases: 
        cases single_case
        |single_case


single_case:
         Case  diff_expressions Colon  expression     
        |Case  diff_expressions  Colon Right_Bracet expressions Break Semicolon  Left_Bracet 
        |Default  Colon  expression
        |Default  Colon Right_Bracet expressions Break Semicolon Left_Bracet 
        |Default  Colon Right_Bracet  Break Semicolon Left_Bracet 

for_exp:
        For Right_par for_paranthesis Left_par Right_Bracet expressions Left_Bracet 
        |For Right_par for_paranthesis Left_par  expression                             ///{$$=strcat(strcat(strcat(strcat("for", "("), $3), ")"), $5);}

for_paranthesis:
                 var_assignment Semicolon  diff_expressions  Semicolon diff_expressions ///{$$=strcat(strcat(strcat(strcat($1, $2), $3), $4), $5);}

diff_expressions:       //represent mostly conditions 
        operations_expressions                                  {$$=$1;}
        | Right_par diff_expressions Left_par                   ///{$$=strcat(strcat("(", $2), ")");}
        | fnc_call                                              {$$=$1;}
        | Letters                                               {
                                                                $$=$1;
                                                                add_arg(args, &args_index, $1);
                                                                }
        | INTEGER                                               {$$=$1; 
                                                                add_arg(args, &args_index, $1);
                                                                }  
        | FLOATS                                                {$$=$1; 
                                                                add_arg(args, &args_index, $1);
                                                                }        
        | True                                                  {
                                                                $$="True"; 
                                                                add_arg(args, &args_index, "1");
                                                                }     
        |False                                                  {
                                                                $$="False"; 
                                                                add_arg(args, &args_index, "0");
                                                                }     
        |Single_quotes Letters Single_quotes                    {add_arg(args, &args_index, $2);}

operations_expressions:
                or_expr            {$$=$1; add_op(ops, &op_index, OR);}
                |and_expr           {$$=$1; add_op(ops, &op_index, AND);}
                |equality_expr         {$$=$1;}
                |relational_expr       {$$=$1;}
                |binary_expr           {$$=$1;}
                
or_expr:
        diff_expressions Or diff_expressions                    ///{$$=strcat(strcat($1, "||"), $3);}  
       
and_expr:
         diff_expressions And diff_expressions                  ///{$$=strcat(strcat($1, "&&"), $3);}  
                    
equality_expr:
        diff_expressions equality diff_expressions             ///{$$=strcat(strcat($1, $2), $3);}  
              
relational_expr: 
                diff_expressions relational  diff_expressions  ///{$$=strcat(strcat($1, $2), $3);}  

relational :            Biggerthan                      {add_op(ops, &op_index, GT);} ///{$$=">";}
                      | Smallerthan                     {add_op(ops, &op_index, LT);} ///{$$="<";}
                      | Biggerthanequal                 {add_op(ops, &op_index, GTE);} ///{$$=">=";}
                      | Smallerthanequal                {add_op(ops, &op_index, LTE);}///{$$="<=";};                         
binary_expr:
                diff_expressions '*'  diff_expressions  {
                                                        add_op(ops, &op_index, MUL);
                                                        check_math_operations($1,$3);
                                                        }
                |diff_expressions '/'  diff_expressions  {
                                                        add_op(ops, &op_index, DIV);
                                                        check_math_operations($1,$3);
                                                        }
                |diff_expressions '%'  diff_expressions  {
                                                        add_op(ops, &op_index, MOD);
                                                        check_math_operations($1,$3);
                                                        }
                |diff_expressions '+'  diff_expressions   {
                                                        add_op(ops, &op_index, ADD);
                                                        check_math_operations($1,$3);
                                                        }
               |diff_expressions '-'  diff_expressions  {
                                                        add_op(ops, &op_index, SUB);
                                                        check_math_operations($1,$3);
                                                        }
               |Increment diff_expressions              ///{$$=strcat("++ ", $2);}
               |Decrement  diff_expressions             ///{$$=strcat("-- ", $2);}
               |diff_expressions Increment              ///{$$=strcat($1, " ++");}
               |diff_expressions Decrement              ///{$$=strcat($1, " --");}

        //        diff_expressions multiplication diff_expressions   ///{$$=strcat(strcat($1, $2), $3);}


// multiplication: 
//                       '*' {add_op(ops, &op_index, MUL);}
//                       | '/' {add_op(ops, &op_index, DIV);}
//                       | '%' {add_op(ops, &op_index, MOD);};
 

 
equality : 
                    Eq                  {add_op(ops, &op_index, EQ);}///{$$="=";}
                    |Noteq              {add_op(ops, &op_index, NEQ);}///{$$="!=";}
                    ;




/*
func1:
        INTEGER
        | expr '+' expr           ///{ $$ = $1 + $3; }
        | expr '-' expr           ///{ $$ = $1 - $3; }
        ;
*/
%%
int valid=1;
void yyerror(char *s) {
 //   fprintf(stderr, "%s\n", s);
   valid=0;
 char * e;
if (yychar == 272  )
	e ="missing semicolon";
else if (yychar == 0)
	e ="Unmatched brackets";
else if(yychar ==258)
        e="Invalid Identifier ";
else if (yychar == 287)
	e ="Unmatched brackets";
else if (yychar == 286)
	e ="Unmatched paranthesis";
else if (yychar == 259)
	e ="Unmatched paranthesis";

else 
e = "";

    fprintf(stderr, "syntax error at line %d: %s \n", yylineno, e);
    fprintf(fw, "syntax error at line %d: %s \n", yylineno, e);
	// yyerrok(); 
}
extern FILE *yyin;

int main(int argc, char *argv[]) {

        yydebug = 0;
char file_name[25] ;
//scanf("%s", file_name); 


    if ((yyin = fopen(argv[1],"r")) == NULL)
    {
        fprintf(stderr,"File not found or not readable.\n");
    }else{
   
  fw = fopen ("result.txt","w");
  ass=fopen("assembly.txt","w");
    yyparse();
 
    

      if(valid ==1){
      fprintf(fw,"Input program is valid.\n");
      }
      else{
      fprintf(fw,"Input program is invalid.\n");
      }
   display(symbols,table_index,fw);}
        char main_assembly[10000] = "";
        compile(main_assembly, all_expr, all_expr_type, expr_idx);
        fprintf(ass,"##################################\n");
        fprintf(ass,"Assemply:\n%s", main_assembly);
        fprintf(ass,"##################################\n");
        
        return 0;
}
int yywrap(void) {
    return 1;
}
    void inc_level(){
level ++;
    }
    void dec_level(){
           
level --;

    }
     void inc_scope_id(){
scope_id ++;
     }
int check_compatible(const char* name,const char*  name2){
        char type1=get_identifier(name,symbols,table_index,yylineno,level,scope_id,fw);
          char type2=get_identifier(name2,symbols,table_index,yylineno,level,scope_id,fw);
        if(type1 == type2)
        return 1;
        else{
                fprintf(fw,"Error at line number %d:types don't match \n",yylineno,fw);
        }
        return 0;
}
int check_math_operations(const char* name,const char*  name2){
          char type1=get_identifier(name,symbols,table_index,yylineno,level,scope_id,fw);
          char type2=get_identifier(name2,symbols,table_index,yylineno,level,scope_id,fw);
        if(type1==type2)
        return 1 ;
        else{
                fprintf(fw,"Error at line number %d: can not perform mathematical operation ,types don't match \n",yylineno,fw);
        }
return 0;
}

void print_string(const char* s){
        fprintf(ass,"testing %s", s);
}

void print_value(char* s){
        fprintf(ass,"code = %s\n", s);
}