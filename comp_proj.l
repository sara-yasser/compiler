%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "y.tab.h"
    void yyerror(char *);
 
%}
%option yylineno
DIGIT    [0-9]
NUMBER   [-]?[0-9]+
alphabet [a-zA-Z]
LETTERS {alphabet}({alphabet}|{DIGIT})*
FLOAT [-]?[0-9]*(\.)[0-9]+
newline [\n]
%%

"int"	    return Int;
"float"	  return Float;
"string"  return String;
"char"	  return Char;
"bool"    return Bool;     
"void"    return Void;
"true"    return True;
"false"   return False;



"if"       return If;
"else"     return Else;
"while"    return While;
"do"       return Do;
"for"      return For;
"switch"   return Switch;
"case"     return Case;
"default"  return Default;
"break"    return Break;
"continue" return Continue;
"return"   return Return;

">"			return Biggerthan;
"<"			return Smallerthan;
"=="		return Eq;
"!="		return Noteq;
">="		return Biggerthanequal;
"<="		return Smallerthanequal;
"&&"		return And;
"||"		return Or;

"="     {  return Ass_operator;}

"++"		return Increment;
"--"		return Decrement;
","          return Comma;
";"           {return Semicolon;}
":"            return Colon;
"{"          { inc_level();inc_scope_id();return Right_Bracet;}
"}"         {dec_level(); return Left_Bracet;}
"("          return Right_par;
")"          return Left_par;
"const"      return Constant;
"printf"     return Print;
"'"         return Single_quotes;





{NUMBER}  { yylval.STRNG=strdup(yytext); return INTEGER;}
{LETTERS} { yylval.STRNG=strdup(yytext); return Letters;}
{FLOAT}   { yylval.STRNG=strdup(yytext); return FLOATS;}
{newline}     ;

[-+()=/*\n]      { return *yytext; }

[ \t]       ;       /* skip whitespace */

.           yyerror("Unknown character");
<<EOF>>      { yyterminate(); return 0; }




%%

