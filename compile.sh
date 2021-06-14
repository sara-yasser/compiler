rm y.tab.h
rm y.tab.c
rm lex.yy.c
rm a.out
bison -y comp_proj.y -d
flex comp_proj.l
gcc y.tab.c lex.yy.c
./a.out
