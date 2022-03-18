#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lex.h"
#include "token.h"
#include "parser.h"
#include "scanner.h"
#include "string.h"

extern char* fileName;
char* parser = "parser";
char nextChar;
struct token * nextTok;
FILE * fp;
int epsilon_flag = 0;

void printParserError(char* fmt, ...)
{
	char buf[PARABUFF];
	va_list args;

	va_start(args,fmt);
	vsprintf(buf,fmt,args);
	va_end(args);
	
	printRed();
	printf("%s:%d:%d: PARSER ERROR: ",fileName,nextTok->line,nextTok->charN);
	printYellow();
	printf(buf);
	fflush(stdout);
	printReset();	
	exit(-1);
}

int isfileEmpty(FILE * fp, char* caller)
{
        if(fp != NULL)
        {
                fseek(fp, 0, SEEK_END);//set the pointer to the end of the file
                int test = ftell(fp);

                if(test == 0)
                {
                        fprintf(stderr, "ERROR: %s: Input is empty\n", caller);
                        return 1;
                }


        }
        else
        {
                fprintf(stderr, "ERROR: %s: Cannot open file\n", caller);
                return 1;
        }

        fseek(fp, 0, SEEK_SET); //reset the pointer back to the beginning of the file

        return 0;
}
void scanner()
{
	if(epsilon_flag == 0){
		nextTok = FSADriver(fp);
		while(nextTok->tokenID == COMMENT)
			nextTok = FSADriver(fp);
	}else
		epsilon_flag = 0;
	return;	
}
int matching(int Token, char* TokenIns)
{
	if(nextTok->tokenID == Token && (TokenIns == NULL || strcmp(nextTok->tokenIns,TokenIns) == 0))
			return 1;

	return 0;

}
int runParser()
{
	fp = fopen(fileName, "r");
	if(isfileEmpty(fp,parser) == 1)
		return 0;
	nextChar = fgetc(fp);
	
	scanner();	
	program();	

	fclose(fp);
	free(nextTok->tokenIns);
	free(nextTok);
	return 1; //the program run successfully
}

void program()
{
	vars();

	scanner();
	if(matching(KEYWORD,"main") == 0)
		printParserError("Expected 'main' token, but received '%s'\n",nextTok->tokenIns);
	
	scanner();
	block();	
	return;
}
void block()
{
	if(matching(OPERATOR,"{") == 0)
		printParserError("Expected a block of statements, but received '%s'\n",nextTok->tokenIns);
	
	scanner();
	vars();
	
	//scanner();
	//stats();

	scanner();
	if(matching(OPERATOR,"}") == 0)
		printParserError("Reach the end of non-closing block, received %s\n",nextTok->tokenIns);

	return;		

}
void vars()
{
	//create a node and add all token to the node
	if(matching(KEYWORD,"declare") == 1){
		scanner();
		if(matching(IDENT, NULL) == 0)
			printParserError("Expected an Identifier token after 'declare' token\n");
		scanner();
		if(matching(OPERATOR,":=") == 0)
			printParserError("Expected ':=', but received '%s'\n",nextTok->tokenIns);
		
		scanner();
		if(matching(KEYWORD,"whole") == 0)
			printParserError("Expected data type 'whole', but received '%s'\n",nextTok->tokenIns);
		
		scanner();
		if(matching(OPERATOR,";") == 0) 
                        printParserError("Expected ';' before '%s' tokeni\n",nextTok->tokenIns);
	
		scanner();
		vars();
						
	}else{
		epsilon_flag = 1;
		return; //encounter epsilon
	}
	return; //should return a node then
}
void stats()
{
		


}


















