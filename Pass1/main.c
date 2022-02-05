#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_MEMORY 32768  //this is the max memory for SIC 2^15
#define MAX_WORD 8388608  // this is the max word count we can have in SIC 2^24

// here i have created a static array all the instruction for the SIC. 
static const char *Opcode_Table[]=
{"ADD","ADDF","ADDR","AND","CLEAR","COMP","COMPF","COMPR","DIV","DIVF","DIVR","FIX","FLOAT","HIO","J","JEQ","JGT","JLT","JSUB","LDA","LDB","LDCH","STB","LDF","LDL","LDS",
 "LDT","LDX","LPS","MUL","MULF","MULR","NORM","OR","RD","RMO","RSUB","SHIFTL","SHIFTR","SIO","SSK","STA","STCH","STF","STI","STL","STS","STSW","STT","STX","SUB","SUBF",
    "SUBR","SVC","TD","TIO","TIX","TIXR","WD"
};




int main( int argc, char* argv[]){
	

	FILE *fp; // this reads the file name
	char line[1024]; // this is to through line by line from file
	int counter; // this count for memory from the file
	int lines =0; // this keep tracks of how many lines are been read on file and where the error is from the file
	char* newsym; // the stores new symbol from the file
	char* opcode; // this stores opcode from the file
	char* oparand; // this stores the memory address from the file 
	char* token; // this goes through each token and stores them into proper place 
	char* token_2;
	
	char fullline[1024]; //this stores the full lines from file 
	if ( argc != 2 ) {
	printf("ERROR: Usage: %s filename\n", argv[0]);
	return 0;
	}


	fp = fopen( argv[1], "r");

	if ( fp == NULL ) 
	{
		printf("ERROR: %s could not be opened for reading,\n", argv[1] );
		return 0;
	}
	
	// here setting all the memory to zero in order to make sure we dont run into segment fault error 
	// malloc creates the sizes for the char and memset NULL out everything. 
	newsym = malloc(  1024 * sizeof(char));	
	memset( newsym, '\0', 1024 * sizeof(char));

	opcode = malloc(  1024 * sizeof(char));	
	memset( opcode, '\0', 1024 * sizeof(char));

	oparand= malloc(  1024 * sizeof(char));	
	memset( oparand,'\0', 1024 * sizeof(char));

	token_2=token= malloc(  1024 * sizeof(char));	
	memset(token,'\0', 1024 * sizeof(char));
	
	// creating a symbol table array for the pointer to create a symbol table. 
	SYMBOL *Symbol_table[1024];
	memset(Symbol_table, '\0' ,1024 * sizeof(SYMBOL*));
#ifdef DEBUG
			printf("outside %p\n",&token);
			printf("outside %p\n",&token_2);
			printf("outside %p\n",&newsym);
			printf("outside %p\n",&opcode);
			printf("outside %p\n",&oparand);
#endif
	

	// in this while loop we go through line by line and see what we need to do with that line
	while(  fgets( line , 1024 , fp ) !=NULL) {
		lines ++; 
		strcpy( fullline, line ); // each line that currenctly is beign read is get coppied to fullline for storage in order to print it out for the error.
		if ( line[0] == 35) {  // the 35 is an ASCII number for #
			//printf("COMMENT:%s", line );
			continue;
		}	
		 if((line[0]>=65) &&(line[0]<=90)) // the 65 and 90 is an ASCII number for alphabate A to Z
		{
			token = strtok(line," \t\n\r"); // the first token is read 

#ifdef DEBUG
			printf("01 %p\n",&token);
#endif
			if(IsAValidSymbol(token)==0) // checking for error 
			{
				printf("ERROR. INVALID SYMBOL\n");
				fclose(fp);
				free(newsym);
				free(opcode);
				free(oparand);
				free(token_2);
				for(int ab=0;Symbol_table[ab];ab++)
				{
				free(Symbol_table[ab]);
				}
				return 0;
			}
			else
			{
				strcpy(newsym,token); // the first token is store in symbol table 
				
#ifdef DEBUG	
				printf("1 first newsym memory %p\n", &newsym);
#endif
				token = strtok(NULL," \t\n\r"); // this moves to next token 
#ifdef DEBUG
				printf("01 %p\n", &token);
#endif
			}
			if(IsADirective(newsym)==1) // if the symbol is a directive then it thorugh an error 
			{
				printf("Testing case %s SIC Program Has Symbol Name that is an assembler Directive %s\n",argv[1],newsym);
				printf("It is on line %d and line is %s",lines,fullline);
				free(newsym);
				free(opcode);
				free(oparand);
				free(token_2);
				for(int ab=0;Symbol_table[ab];ab++)
				{
				free(Symbol_table[ab]);
				}
				fclose(fp);
				return 0;
			}
			if(checkOpcode(newsym)==1) // if the symbol is an instruction then it thorugh an error 
			{
				printf("Testing case %s SIC Program Has Symbol Name that is an assembler Instruction %s\n",argv[1],newsym);
				printf("It is on line %d and line is %s",lines,fullline);
				free(newsym);
				free(opcode);
				free(oparand);
				free(token_2);
				for(int ab=0;Symbol_table[ab];ab++)
				{
				free(Symbol_table[ab]);
				}
				fclose(fp);
				return 0;
			}
			if(IsADirective(token)==0) // now it check if the next token is directive or instruction if it is neither than it through an error 
			{
				if(checkOpcode(token)==0)
				{
				printf("this is not a token 137 %s\n",token);
				free(newsym);
				free(opcode);
				free(oparand);
				free(token_2);
				for(int ab=0;Symbol_table[ab];ab++)
				{
				free(Symbol_table[ab]);
				}
				fclose(fp);
				return 0;
				}
			}
#ifdef DEBUG
			printf("01 %p\n", &token);
#endif
			strcpy(opcode,token); // if the token passes the error test than it is copied to opcode 
#ifdef DEBUG_OPCODE
			printf("1 first opcode memory %p\n", &opcode);
			printf("line 142 %s\n",opcode);
#endif	 
			token = strtok(NULL,"\t\n\r"); // this moves to next token and also i have removed the delimeter space because it was causing an error while reading address
#ifdef DEBUG
			printf("01 %p\n", &token);
#endif
			strcpy(oparand,token); // this copies memory address into oparand
			
#ifdef DEBUG
			printf("1 first oparand memory %p\n", &oparand);
			printf("oparand %s\n",oparand);
#endif
			token = strtok(NULL," \t\n\r"); 
			if(strcmp(opcode,"START")==0) //if the opcode is a START from the file then from here the memory address is added to counter for calculation
			{
				counter = strtol(oparand,NULL,16); // strtol converts the string into the long integer and it is assigned to counter 
				if(counter == MAX_MEMORY) // here i am checking if the memory where it start is already at max if so it through an error and stops the code
				{
					printf("Testing case %s SIC Program Starts at Hex %04X No room left in SIC memory.\n",argv[1],counter);
					free(newsym);
					free(opcode);
					free(oparand);
					free(token_2);
					for(int ab=0;Symbol_table[ab];ab++)
					{
					free(Symbol_table[ab]);
					}
					fclose(fp);
					return 0;
				}
			}
			
			//Symbol_Exit(Symbol_table,newsym);
			if(Symbol_Exit(Symbol_table,newsym)==-1) // if the symbol already exits in symbol table then it through an error 
			{
				printf("Test Case %s SIC Program Has %s defind twice\n",argv[1],newsym);
				printf("It is on line %d and line is %s",lines,fullline);
				free(newsym);
				free(opcode);
				free(oparand);
				free(token_2);
				for(int ab=0;Symbol_table[ab];ab++)
				{
				free(Symbol_table[ab]);
				}
				fclose(fp);
				return 0;
				
			  }
			else 
			{
				AddSymbols(Symbol_table,counter,lines,newsym); // if symbol doesnt exit in symbol table then it is added 
			}
			
		}
		else if(line[0]==9) // the 9 is an ASCII number for tab if the symbol contains tab then it stright goes to oparand.
		{
#ifdef DEBUG
			printf("11 %p\n", &token);
#endif
			token = strtok(line," \t\n\r"); 
#ifdef DEBUG
			printf("11 %p\n", &token);
#endif
			if(IsADirective(token)==0)
			{
				if(checkOpcode(token)==0)
				{
					printf("this is not a token 137 %s\n",token);
					free(newsym);
					free(opcode);
					free(oparand);
					free(token_2);
					for(int ab=0;Symbol_table[ab];ab++)
					{
					free(Symbol_table[ab]);
					}
					fclose(fp);
					return 0;
				}
			}
#ifdef DEBUG
			printf("11 %p\n", &token);
#endif
			strcpy(opcode,token);
#ifdef DEBUG
			printf("1 first tab opcode memory %p\n", &opcode);
#endif
			token = strtok(NULL,"\t\n\r");
#ifdef DEBUG
			printf("11 %p\n", &token);
#endif
			while(token!=NULL) // which copies full memory address to oparand when there is a space contain. 
			{
#ifdef DEBUG
			printf("21 %p\n", &token);
#endif
				strcpy(oparand,token);
#ifdef DEBUG
			printf("1 first tab in while loop oparand memory %p\n", &oparand);
#endif
				//printf("line 131 %s\n",oparand);
				token = strtok(NULL," \t\n\r");
#ifdef DEBUG
			printf("21 %p\n", &token);
#endif
			}
		}
		else //this check if the first symbol has any invilid symbol or if the file contains blank space in the file. if so then it through an error.
		{
			for(int i =0;i< strlen(line);i++)
			{
			if( (line[i]==36) || (line[i]==33) || (line[i]==37) ||(line[i]==61) || (line[i]==43) || (line[i]==45)
				||(line[i]==40) || (line[i]==41) || (line[i]==64))
			{
				printf("Testing case %s Program Has Invalid Symbol Name\n",argv[1]);
				printf("Full Invilid symbol is on line %d line is %s",lines,fullline);
				free(newsym);
				free(opcode);
				free(oparand);
				free(token_2);
				for(int ab=0;Symbol_table[ab];ab++)
				{
				free(Symbol_table[ab]);
				}
				fclose(fp);
				return 0;
			}
			}
				printf("Testing case %s Has blank lines on line %d\n",argv[1],lines);
				free(newsym);
				free(opcode);
				free(oparand);
				free(token_2);
				for(int ab=0;Symbol_table[ab];ab++)
				{
				free(Symbol_table[ab]);
				}
				fclose(fp);
				return 0;
		}
		
		if(counter>MAX_MEMORY) // checking if the memory is bigger than max memeory if so then it through an error.
		{
			printf("Testing case %s Program Does not fit in SIC Memory, Memory is too big %04X\n",argv[1],counter);
			free(newsym);
			free(opcode);
			free(oparand);
			free(token_2);
			for(int ab=0;Symbol_table[ab];ab++)
			{
			free(Symbol_table[ab]);
			}
			fclose(fp);
			return 0;

		}
		if(strcmp(opcode,"END")!=0) // this check if the opcode is equall to END if so then this all calculation stops. 
		{
		if(checkOpcode(opcode)==1) // if the given opcode is equall to oparand then the counter is incresed by 3
		{
			//printf("line 184 %04X\n",counter);
			counter += 3;
		}
		else if(strcmp(opcode,"BYTE")==0) // this checks if the opcode is byte. then it check if the address starts with C or X
		{
			if(oparand[0]==67) // the 67 is ASCII number for C
			{
				int c = 2;
				while(oparand[c++]!='\''); // this counts all the bytes in C between two quotes 
				c -=3;
				if(c>20)
				{
					printf("Long Byte Constant is %s\n",oparand);
				}
				//printf("this is for the C %d\n",c);
				counter +=c;
			}
			else if(oparand[0]==88) // the 88 is ASCII for X
			{
				int x =2;
				char hex[10]; // this sotes everything from X between two quotes 
				while(oparand[x]!='\'')
				{
					hex[x-2]=oparand[x];
					x++;
				}
				hex[x-2]='\0';
				//printf("%d hex return\n",check_for_hex(hex));
				if(check_for_hex(hex)==0) //this check for invild hex digit.
				{
					printf("Testing case %s SIC Program Has Invalid Hex Constant\n",argv[1]);
					printf("The Invilid hex on line %d is %s",lines,fullline);
					free(newsym);
					free(opcode);
					free(oparand);
					free(token_2);
					for(int ab=0;Symbol_table[ab];ab++)
					{
					free(Symbol_table[ab]);
					}
					fclose(fp);
					return 0;
				}
				else
				{
					int X = strlen(hex);
					//printf("line 196 %d\n",X);
					counter += (X/2);
				}
			}
		}
		else if(strcmp(opcode,"WORD")==0) // this check if the opcode is word or not.
		{
			int oparand_int = atoi(oparand); // here it converts string to integer in order to check for the error
			if(oparand_int >= MAX_WORD)
			{
				printf("Testing case %s WORD Contant Exceeds 24 bit limitation\n",argv[1]);
				free(newsym);
				free(opcode);
				free(oparand);
				free(token_2);
				for(int ab=0;Symbol_table[ab];ab++)
				{
				free(Symbol_table[ab]);
				}
				fclose(fp);
				return 0;
			}
			counter +=3;
		}
		//this check if the opcode is resb. if it is then the memory address is converted from string to integer and added to counter 
		else if(strcmp(opcode,"RESB")==0) 
			
		{
			counter +=atoi(oparand);
		}
		//this check if the opcode is resw. if it is then the memory address is converted from string to integer and multipled by 3 then added to counter 
		else if(strcmp(opcode,"RESW")==0)
		       	
		{
			counter = counter +(3* atoi(oparand)); 
		}
		}
	}
	
	print_table(Symbol_table);
	free(newsym);
	free(opcode);
	free(oparand);
	free(token_2);
	for(int ab=0;Symbol_table[ab];ab++)
	{
		free(Symbol_table[ab]);
	}
	
	fclose( fp );
	return 0;
	
}

// this checks is given token is oparand or not. if the given token is oparand then it returns 1 else 0
int checkOpcode(char *test)
{
	for(int i=0;i<59;i++)
	{
		if(strcmp(test,Opcode_Table[i])==0)
		{
			//printf("it is in the table %s\n",test);
			return 1;
		}
	}
		return 0;
}

// here it checks if the given symbol already exits in symbol table.
int Symbol_Exit(SYMBOL *Tab[],char *Sname)
{
	int index =0;
	int result;
	result=0;

	while(Tab[index]!=NULL )
	{
		if(strcmp(Tab[index]->Name,Sname) ==0)
		{
			result =-1;
			break;
		}
		index++;
	}
	return result;
}

// here all the new symbols are added to symbol table with their address, sorce line and name.
void AddSymbols(SYMBOL *Tab[],int Addr,int Srcline,char *Sname)
{
	int index =0;
	SYMBOL *temp;
	temp = malloc(sizeof(SYMBOL));
	temp->Address=Addr;
	temp->DefinedOnSourceLine=Srcline;
	strcpy(temp->Name,Sname);
	while(Tab[index]!=NULL)
	{
		index++;
	}
	Tab[index]=temp;
	
}

// this just prints out symbol table
void print_table(SYMBOL *Tab[])
{
	int index =0;
	while(Tab[index]!=NULL)
	{
		printf("%s\t\t %X\n",Tab[index]->Name,Tab[index]->Address);
		index++;
	}

}


//this checks if the given array contains invild hex digit if does then it returns -1 else 0 
int check_for_hex(char hex[])
{
	int result;
	for(int i=0;i<strlen(hex);i++)
	{
		if( (hex[i]>=48 && hex[i]<=57) || (hex[i]>=65 && hex[i]<=70) || (hex[i]>=97 && hex[i]<=102))
		{
			result =1;
		}
		else
		{
			result=0;
		}
	}
	return result;

}


