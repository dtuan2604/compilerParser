#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "token.h"
#include "parser.h"
#include "scanner.h"

char* testScanner = "testScanner";
char nextChar;
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
int runParser(char* fileName)
{
	struct token * tok;
	FILE * fp = fopen(fileName, "r");
	if(isfileEmpty(fp,testScanner) == 1)
		return 0;
	nextChar = fgetc(fp);
	//Call scanner here
	while(1)//run until encounter EOF or error
	{
		tok = FSADriver(fp);
		if(tok->tokenID < ERROR)
		{
			printError(tok);
			return -1; //encounter errors			
		}else{
			printToken(tok);
			if(tok->tokenID == EndOfFile)
				break;
		}
	}

	fclose(fp);
	free(tok->tokenIns);
	free(tok);
	return 1; //the program run successfully
}
