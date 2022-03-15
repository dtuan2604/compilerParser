#ifndef _SCANNER_H
#define _SCANNER_H
#include "token.h"

#define NUM_ERRORS 3 

void printError(struct token * tok);
void printToken(struct token * tok);

int isKeyword(char* identifier);
int isOperator(char character);
int charToFSAIndex(char character);
struct token * FSADriver(FILE* fp);
#endif
