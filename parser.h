#ifndef _PARSER_H
#define _PARSER_H
#include "token.h"
#include "tree.h"
#define PARABUFF 500

void scanner();
int matching(int Token, char* TokenIns);

void printParserError(char* fmt, ...);

int isfileEmpty(FILE* fp, char* caller);
void copyToken(struct node_t ** newNode);
struct node_t * runParser();

struct node_t * program();
struct node_t * block();
struct node_t * vars();

void stats();
void stat();
void mStat();

void in();
void out();
void if_();
void gen_loop();
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
