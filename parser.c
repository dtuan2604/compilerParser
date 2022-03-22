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
struct node_t * runParser()
{
	struct node_t * tempRoot = NULL;
	fp = fopen(fileName, "r");
	if(isfileEmpty(fp,parser) == 1)
		return 0;
	nextChar = fgetc(fp);
	
	scanner();	
	tempRoot = program();	

	fclose(fp);
	free(nextTok->tokenIns);
	free(nextTok);
	return tempRoot; //the program run successfully
}
void copyToken(struct node_t ** newNode){
	int index = (*newNode)->numToken;
	struct token * tempToken = (*newNode)->tokenList[index];
	tempToken->tokenID = nextTok->tokenID;
	tempToken->tokenIns = (char*)malloc(sizeof(nextTok->tokenIns));
	if(tempToken->tokenIns == NULL){
		fprintf(stderr,"ERROR: %s: Cannot allocate memory for token instance\n", parser);
		exit(-1);
	}
	strcpy(tempToken->tokenIns, nextTok->tokenIns);
	tempToken->line = nextTok->line;
	tempToken->charN = nextTok->charN;
	(*newNode)->numToken++; 

}
struct node_t * program()
{
	struct node_t * tempNode = createNode("program");
	tempNode->left = vars();

	scanner();
	if(matching(KEYWORD,"main") == 0)
		printParserError("Expected 'main' token, but received '%s'\n",nextTok->tokenIns);
	else
		copyToken(&tempNode);

	scanner();
	tempNode->right = block();	
	return tempNode;
}
struct node_t * block()
{
	struct node_t * tempNode = createNode("block");
	if(matching(OPERATOR,"{") == 0)
		printParserError("Expected a block of statements, but received '%s'\n",nextTok->tokenIns);
	else
		copyToken(&tempNode);

	scanner();
	tempNode->left = vars();
	
	scanner();
	stats();

	scanner();
	if(matching(OPERATOR,"}") == 0)
		printParserError("Reach the end of non-closing block, received %s\n",nextTok->tokenIns);
	else
		copyToken(&tempNode);
	return tempNode;		

}
struct node_t * vars()
{
	struct node_t * tempNode = createNode("vars");
	//create a node and add all token to the node
	if(matching(KEYWORD,"declare") == 1){
		copyToken(&tempNode);	
	
		scanner();
		if(matching(IDENT, NULL) == 0)
			printParserError("Expected an Identifier token after 'declare' token\n");
		else
			copyToken(&tempNode);

		scanner();
		if(matching(OPERATOR,":=") == 0)
			printParserError("Expected ':=', but received '%s'\n",nextTok->tokenIns);
		else
			copyToken(&tempNode);
		
		scanner();
		if(matching(KEYWORD,"whole") == 0)
			printParserError("Expected data type 'whole', but received '%s'\n",nextTok->tokenIns);
		else
			copyToken(&tempNode);
		
		scanner();
		if(matching(OPERATOR,";") == 0) 
                        printParserError("Expected ';' before '%s' token\n",nextTok->tokenIns);
		else
			copyToken(&tempNode);
	
		scanner();
		tempNode->left = vars();
						
	}else{
		epsilon_flag = 1;
	}
	return tempNode; //should return a node then
}
void stats()
{
	stat();

	scanner();
	mStat();
	
	return;			

}
void stat()
{
	int block_flag = 0;
	if(matching(KEYWORD, NULL) == 1 || matching(OPERATOR, "{") == 1){
		if(strcmp(nextTok->tokenIns,"listen") == 0)
			in();
		else if(strcmp(nextTok->tokenIns,"yell") == 0)
			out();
		else if(strcmp(nextTok->tokenIns,"if") == 0)
			if_();
                else if(strcmp(nextTok->tokenIns,"repeat") == 0)
			gen_loop();
                else if(strcmp(nextTok->tokenIns,"assign") == 0)
			assign();
                else if(strcmp(nextTok->tokenIns,"portal") == 0)
			goto_();
                else if(strcmp(nextTok->tokenIns,"label") == 0)
			label();
                else if(strcmp(nextTok->tokenIns,"{") == 0){
			block();
			block_flag = 1;
		}
	}else
		printParserError("Expected a statement, but received '%s' token\n", nextTok->tokenIns);
	
	scanner();
	if(block_flag == 0){
		if(matching(OPERATOR,";") == 0)  
                        printParserError("Expected ';' before '%s' token\n",nextTok->tokenIns);	
	}else
		epsilon_flag = 1; //assuming after <block> terminal is an epsilon character
	return;
}
void mStat()
{
	if(matching(KEYWORD,NULL) == 1 || matching(OPERATOR,"{") == 1){
		stat();

		scanner();
		mStat();
	}else{
		epsilon_flag = 1; // received an epsilon token
		return;
	}

	return;
}
void in()
{
	if(matching(KEYWORD,"listen") == 0)
		printParserError("Do you mean 'listen' ? (Received '%s' token)\n",nextTok->tokenIns);
	
	scanner();
	if(matching(IDENT,NULL) == 0)
		printParserError("Expected an identifier, but received '%s' token\n",nextTok->tokenIns);

	return; 
}
void out()
{
	if(matching(KEYWORD,"yell") == 0)
                printParserError("Do you mean 'yell' ? (Received '%s' token)\n",nextTok->tokenIns);
    
        scanner();
	expr();
	
        return; 

}
void condition() 
{
	if(matching(OPERATOR, "[") == 0)
        	printParserError("Expected '[' before '%s' token\n",nextTok->tokenIns);

        scanner();
        expr();

        scanner();
        RO();

        scanner();
        expr();

        scanner();
        if(matching(OPERATOR, "]") == 0)
                 printParserError("Expected ']' before '%s' token\n",nextTok->tokenIns);

	return;
}
void if_()
{
	if(matching(KEYWORD, "if") == 0)
		printParserError("Expected 'if' token for conditional statement but received '%s'\n",nextTok->tokenIns);
	
	scanner();
	condition();	
	
	scanner();
	if(matching(KEYWORD, "then") == 0)
                 printParserError("Expected 'then' token before '%s' token\n",nextTok->tokenIns);

	scanner();
	stat();

	return; 

}
void gen_loop()
{
	if(matching(KEYWORD, "repeat") == 0)
                 printParserError("Expected 'repeat' token for loop statement, but received '%s'\n",nextTok->tokenIns);
	
	scanner();
	if(matching(OPERATOR,"[") == 1){
		loop1();
	}else
		loop2();
	
	return;
}
void loop1()
{
	condition();

	scanner();
	stat();

	return;
}
void loop2()
{
	stat();
	
	scanner();
	if(matching(KEYWORD, "until") == 0)
        	printParserError("Expected 'until' token, but received '%s'\n",nextTok->tokenIns);

	scanner();
	condition();
	
	return;
		
}
void assign()
{
	if(matching(KEYWORD, "assign") == 0)
                 printParserError("Expected 'assign', but received '%s'\n",nextTok->tokenIns);

	scanner();
	if(matching(IDENT, NULL) == 0)
                 printParserError("Expected an identifier, but received '%s'\n",nextTok->tokenIns);

	scanner();
	if(matching(OPERATOR, "=") == 0)
		printParserError("Expected '=' token, but received '%s'\n",nextTok->tokenIns);

	scanner();
	expr();
	
	return;
}
void goto_()
{
	if(matching(KEYWORD, "portal") == 0)
                 printParserError("Expected 'portal' token, but received '%s'\n",nextTok->tokenIns);

	scanner();
	if(matching(IDENT, NULL) == 0)
                 printParserError("Expected an identifier token, but received '%s'\n",nextTok->tokenIns);

	return;
}
void label()
{
	if(matching(KEYWORD, "label") == 0)
                 printParserError("Expected 'label' token, but received '%s'\n",nextTok->tokenIns);

        scanner();
        if(matching(IDENT, NULL) == 0)
                 printParserError("Expected an identifier token, but received '%s'\n",nextTok->tokenIns);

        return;

}
void RO()
{
	if(matching(OPERATOR,NULL) == 1){
		if(strcmp(nextTok->tokenIns,">=") == 0 || 
			strcmp(nextTok->tokenIns,"<=") == 0 ||
			strcmp(nextTok->tokenIns,"==") == 0 ||
			strcmp(nextTok->tokenIns,"!=") == 0)
			//REMINDER: Add token to node
			printf("Compared operator is consumed on line %d \n",nextTok->line);
		else if(strcmp(nextTok->tokenIns,".") == 0){
			scanner();
			if(matching(OPERATOR,".") == 0)
				printParserError("Do you mean '...' ?\n");

			scanner();
                        if(matching(OPERATOR,".") == 0)
                                printParserError("Do you mean '...' ?\n");
		}else
			printParserError("Expected comparision operator, but received '%s'\n",nextTok->tokenIns);

	}else
		printParserError("Expected comparision operator, but received '%s'\n",nextTok->tokenIns);

	return;
}
void expr()
{
	N();

	scanner();
	if(matching(OPERATOR,"-") == 1){
		scanner();
		expr();
	}else{
		epsilon_flag = 1;
	}
	
	return;
}
void N()
{
	A();
	
	scanner();
	N_();
	
	return;
}
void N_()
{
	if(matching(OPERATOR, "+") == 1 || matching(OPERATOR, "/") == 1){
		scanner();
		A();
			
		scanner();
		N_();
	}else{
		epsilon_flag = 1;
	}
		
	return;
}
void A()
{
	M();

	scanner();
	if(matching(OPERATOR,"*") == 1){
		scanner();
		A();
	}else{
		epsilon_flag = 1;
	}
	
	return;
}
void M()
{
	if(matching(OPERATOR,"%") == 1){
		scanner();
		M();
	}else{
		R();
	}
	
	return;
}
void R()
{
	if(matching(OPERATOR,"(") == 1){
		scanner();
		expr();

		scanner();
		if(matching(OPERATOR,")") == 0)
			printParserError("Expected ')' before '%s' token\n",nextTok->tokenIns);
	}else if(matching(IDENT, NULL) == 1 || matching(NUMBER, NULL) == 1){
		//REMINDER: Add token to the node
		printf("Consume token on line %d\n", nextTok->line); 
	}else
		printParserError("Exepcted an expression, or an identifier, or a number, but received '%s'\n",nextTok->tokenIns);

	return;
}
















