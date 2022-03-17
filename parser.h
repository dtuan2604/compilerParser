#ifndef _PARSER_H
#define _PARSER_H
#include "token.h"
#define PARABUFF 500

void scanner();

void printParserError(char* fmt, ...);

int isfileEmpty(FILE* fp, char* caller);
int runParser();

void program();
void vars();

#endif
