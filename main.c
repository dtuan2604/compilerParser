#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "tree.h"

#define BUFF 200

char prog[BUFF];
char* fileName;

int isfileEmpty(FILE* fp, char* caller);


int main(int argc, char** argv)
{
	strcpy(prog,argv[0]);
	fileName = (char *)malloc(BUFF);
	if(fileName == NULL)
	{
                printf("ERROR: %s: failed to allocate memory for filename\n",prog);
               	return EXIT_FAILURE;
       	}		
	memset(fileName,'\0',BUFF);

	if(argc == 2)
	{
		strcpy(fileName, argv[1]);
		strcpy(fileName, argv[1]);

	}
	else if(argc < 2)
	{
		strcpy(fileName,".outputbuffer");
		
		FILE *ptr = fopen(fileName, "w");
		int input;	
	
		if(ptr == NULL){
			fprintf(stderr,"ERROR: %s: Cannot open file\"%s\"\n",prog,fileName);
			return EXIT_FAILURE;
		}

		if((input = getchar()) == EOF)
		{
			fprintf(stderr,"ERROR: %s: Input is empty\n",prog);
			return EXIT_FAILURE;
		}	
		else
		{
			fprintf(ptr,"%c", input);
		}
		
		while((input = getchar()) != EOF)
			fprintf(ptr,"%c",input);	
		
		fclose(ptr);	
	}
	else
	{
		fprintf(stderr,"ERROR: %s: Please provide one file as an argument to the program\n",prog);
		return EXIT_FAILURE;
	}
	
	initROOT();
	
	struct node_t * root = runParser();
	
	printf("Succesfully parse the program\n");
	traversePreOrder(root,0);

	free(fileName);
	fileName = NULL;
	return EXIT_SUCCESS;		
}

