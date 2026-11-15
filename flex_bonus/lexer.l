
%{
#include <iostream>
using namespace std;
%}

%option noyywrap

LETTER      [a-zA-Z]
DIGIT       [0-9]
ID          {LETTER}({LETTER}|{DIGIT})*
NUM         {DIGIT}+(\.{DIGIT}+(E{DIGIT}+)?)?

%%

"int"|"float"|"boolean"|"if"|"else"|"while" {
    cout << yytext << endl;
}

{NUM} {
    cout << "num" << endl;
}

{ID} {
    cout << "id" << endl;
}

"=="|"!="|"<="|">="|"<"|">" {
    cout << "relop" << endl;
}

"=" {
    cout << "assign" << endl;
}

"+"|"-" {
    cout << "addop" << endl;
}

"*"|"/" {
    cout << "mulop" << endl;
}

"("|")"|"{"|"}"|","|";" {
    cout << yytext << endl;
}

[ \t\r\n]+ {
    /* Ignore whitespace */
}

. {
    cerr << "Lexical error: " << yytext << endl;
}

%%

int main() {
    yylex();
    return 0;
}
