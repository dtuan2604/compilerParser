#ifndef _PARSER_H
#define _PARSER_H
#include "token.h"
#define PARABUFF 500

void scanner();
int matching(int Token, char* TokenIns);

void printParserError(char* fmt, ...);

int isfileEmpty(FILE* fp, char* caller);
int runParser();

void program();
void block();
void vars();

void stats();
void stat();
void mStat();

void in();
void out();
void if_();
void loop1();
void loop2();
void assign();
void goto_();
void label();
void condition(); //extra non-terminal
void RO();

void expr();
void N();
void N_();
void A();
void M();
void R();

#endif
