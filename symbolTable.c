#include<stdio.h>
#include<string.h>
#include <ctype.h>
typedef struct symbol
{ 
    const char* name;
    char type;
    const char* value;
    char param_types[10];                         
    const char* param_names[10];
    char return_type;
    int level;
    int scope_id;
    int line;
    int is_constant; //if constant this variable will be =1
}symb_t;

//get type of identifier 
char get_identifier(const char* name,symb_t* symbols,int index,int lineno,int level,int scope_id,FILE*  fw){
//check is its name exists in symbol table
int index_of_var=0;
symb_t var_to_assign;
    int found=0;
    for(int i=0;i<index;i++){
        if(strcmp(symbols[i].name ,name)==0){
        index_of_var=i;
        found=1;
        break;}
    }
     if(found == 0 && isdigit(name[0])==0 &&name!="True" && name !="False"  ){
    fprintf(fw,"error at line number %d: variable %s is not declared \n",lineno,name);
    return '0';}else if(isdigit(name[0])==1){
        return 'i';
    }else if(name =="True"){
        return 'b';
    }
    else if(name =="False"){
        return 'b';
    }
    else{
         var_to_assign=symbols[index_of_var];


if(var_to_assign.level>level){
fprintf(fw,"Error at line number %d: caan not access variable %s because it is out of scope\n",lineno,name);
}
    }

    
    return symbols[index_of_var].type ;
}

const char* fnc_call(const char* name, symb_t* symbols,int index,int lineno,FILE*  fw){
//check if function name exist in the symbol table
symb_t fnc_to_call;
int index_of_var=0;
    int found=0;
    for(int i=0;i<index;i++){
        if(strcmp(symbols[i].name ,name)==0){
        index_of_var=i;
        found=1;
        break;}
    }
     if(found == 0){
    fprintf(fw,"Error at line number %d: function %s is not found \n",lineno,name);}
    else{
//check that nb of parameters passed matches nb of parameters in func definition
 fnc_to_call=symbols[index_of_var]; }
 
if(fnc_to_call.return_type =='i')
return "i";
else if(fnc_to_call.return_type =='s')
return "s";
else if(fnc_to_call.return_type =='f')
return "f";
else if(fnc_to_call.return_type =='c')
return "c";
else if(fnc_to_call.return_type =='b')
return "boolean";
else if(fnc_to_call.return_type =='v')
return "void";
else return 0;

}

//asign or update value to already declared var
int var_assign(const char* name,symb_t* symbols,int index,int lineno,int level,int scope_id,FILE*  fw){
    //if not constant and if declared
    symb_t var_to_assign;
    int index_of_var=0;
    int found=0;
    for(int i=0;i<index;i++){
        if(strcmp(symbols[i].name ,name)==0){
        index_of_var=i;
        found=1;
        break;}
    }
 
    if(found == 0 ){
    fprintf(fw,"error at line number %d: variable %s is not declared \n",lineno,name);}
    else{
        var_to_assign=symbols[index_of_var];
    if(var_to_assign.is_constant ==1){
fprintf(fw,"error at line number %d: variable %s is a COnSTANT\n",lineno,name);}

 
if(var_to_assign.level>level){
fprintf(fw,"error at line number %d: caan not assign a value to variable %s because it is out of scope\n",lineno,name);
}
    }
return 1;
}

int declare(const char* name,char type,symb_t* symbols,int index,int lineno,int level,int scope_id,FILE*  fw ){
symb_t new_var;
new_var.name=strdup(name);
new_var.type=type;
new_var.level=level;
new_var.line=lineno;
new_var.scope_id=scope_id;
new_var.is_constant=0;
symbols[index]=new_var ;
return 1;
}
int declare_initalize_const(const char* name,char type,symb_t* symbols,int index,int lineno,int level,int scope_id,FILE*  fw){
symb_t new_var;
new_var.name=strdup(name);
new_var.type=type;
new_var.level=level;
new_var.line=lineno;
new_var.scope_id=scope_id;
//new_var.value=strdup(value);
new_var.is_constant =1;
symbols[index]=new_var ;
return 1;
}

int fnc_implementation(const char* name,char return_type,symb_t* symbols,int index,int lineno,int level,int scope_id,FILE*  fw){
    
symb_t new_fnc;
new_fnc.name=strdup(name);
new_fnc.level=level;
new_fnc.line=lineno;
new_fnc.scope_id=scope_id;

new_fnc.return_type=return_type;
new_fnc.is_constant =0;
symbols[index]=new_fnc ;


    return 1;
}
int declaring_and_ass_parameters(const char* name,char type,symb_t* symbols,int index,int lineno,int level,int scope_id,FILE*  fw){
 symb_t new_var;
    new_var.name=strdup(name);
    new_var.type=type;
    new_var.level=level+1;
new_var.line=lineno;
new_var.scope_id=scope_id+1;
new_var.is_constant =0;
symbols[index]=new_var;
return 1;


}
void display(symb_t* symbols,int index,FILE*  fw)
{
	for(int i=0;i<index;i++){
     fprintf(fw,"Symbol name %s \n",symbols[i].name);

     if(symbols[i].type =='i')
     fprintf(fw,"Symbol type Integer \n");
     else if(symbols[i].type =='s')
      fprintf(fw,"Symbol type string \n");
      else if(symbols[i].type =='b')
      fprintf(fw,"Symbol type Boolean \n");
      else if(symbols[i].type =='f')
      fprintf(fw,"Symbol type Float \n");
    else if(symbols[i].type =='v')
      fprintf(fw,"Symbol type Void \n");

     fprintf(fw,"Line where symbol was declared %d \n",symbols[i].line);
     fprintf(fw,"Symbol level %d \n",symbols[i].level);
     fprintf(fw,"Symbol scope id %d \n",symbols[i].scope_id);
     fprintf(fw,"-------------\n");
    }
}