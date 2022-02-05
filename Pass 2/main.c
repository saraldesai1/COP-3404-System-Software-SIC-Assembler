/*********************************************************************
* Name:- Saral Desai						     *
* N#:- N01467288						     *
* Project Pass 2						     *
**********************************************************************/


#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_MEMORY 32768  //this is the max memory for SIC 2^15
#define MAX_WORD 8388608  // this is the max word count we can have in SIC 2^24

// here i have created a static array all the instruction for the SIC. 
static OPCODES Opcode_Table[]=
{
  {"18","ADD"},{"58","ADDF"},{"90","ADDR"},{"40","AND"},{"B4","CLEAR"},{"28","COMP"},{"88","COMPF"},
    {"A0","COMPR"},{"24","DIV"},{"64","DIVF"},{"9C","DIVR"},{"C4","FIX"},{"C0","FLOAT"},{"F4","HIO"},
    {"3C","J"},{"30","JEQ"},{"34","JGT"},{"38","JLT"},{"48","JSUB"},{"00","LDA"},{"68","LDB"},{"78","STB"},
    {"70","LDF"},{"08","LDL"},{"6C","LDS"},{"74","LDT"},{"04","LDX"},{"D0","LPS"},{"20","MUL"},{"60","MULF"},
    {"98","MULR"},{"C8","NORM"},{"44","OR"},{"D8","RD"},{"AC","RMO"},{"4C","RSUB"},{"A4","SHIFTL"},
    {"A8","SHIFTR"},{"F0","SIO"},{"EC","SSK"},{"0C","STA"},{"54","STCH"},{"80","STF"},{"D4","STI"},
    {"14","STL"},{"7C","STS"},{"E8","STSW"},{"84","STT"},{"10","STX"},{"1C","SUB"},{"5C","SUBF"},
    {"94","SUBR"},{"B0","SVC"},{"E0","TD"},{"F8","TIO"},{"2C","TIX"},{"B8","TIXR"},{"DC","WD"},{"50","LDCH"}
}
;




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
	int start_address;
	char *Hrecord[1024];
	char *Trecord[1024];
	char *Mrecord[1024];
	char *Endrecord[1024];
	int startDirective = -1;
	int first_instruction_int =-1;
	bool firs_inst_boolean = false;
	
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
	
	memset(Hrecord,'\0',1024*sizeof(char*));
	memset(Trecord,'\0',1024*sizeof(char*));
	memset(Mrecord,'\0',1024*sizeof(char*));
	memset(Endrecord,'\0',1024*sizeof(char*));
	
	
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
				startDirective = 1;
				counter = strtol(oparand,NULL,16); // strtol converts the string into the long integer and it is assigned to counter 
				start_address=counter;

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
		
		else if(line[0]==9 || line[0]==32) // the 9 is an ASCII number for tab if the symbol contains tab then it stright goes to oparand.
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
			if(startDirective != -1)
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

		}
		
		if(strcmp(opcode,"END")!=0) // this check if the opcode is equall to END if so then this all calculation stops. 
		{
		if(checkOpcode(opcode)==1) // if the given opcode is equall to oparand then the counter is incresed by 3
		{
			if(firs_inst_boolean ==false)
			{
				first_instruction_int = counter;
				firs_inst_boolean = true;
			}
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
	if(startDirective == -1)
	{
		printf("Testing case %s is missing START direvtive\n",argv[1]);
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
	
	print_table(Symbol_table);
	int program_length = counter - start_address;
	
	//keeping all the error checking in pass 1 there is only one error checking is happing the instruction symbol doesnt apper in symbol table
	rewind(fp);
	FILE *OutputFile;
	char* fileName=(char*) malloc( (strlen(argv[1]) + 5) * sizeof(char));
	strcpy(fileName,argv[1]);
	strcat(fileName,".obj");
	OutputFile = fopen(fileName,"w");
	
	int address_test=0; // this is so i can do location counting again
	
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
				strcpy(newsym,token); // the first token is store in symbol table 
				token = strtok(NULL," \t\n\r"); // this moves to next token 
			strcpy(opcode,token);
			token=strtok(NULL,"\t\n\r");
			strcpy(oparand,token);
			token=strtok(NULL," \t\n\r");
			//printf("just to make sure it is working %s %s %s\n",newsym,opcode,oparand);
		}
			

			else if(line[0]==9 || line[0]==32) // the 9 is an ASCII number for tab if the symbol contains tab then it stright goes to oparand.
			{	

			token = strtok(line," \t\n\r"); 
			strcpy(opcode,token);
			token = strtok(NULL,"\t\n\r");
			while(token!=NULL) // which copies full memory address to oparand when there is a space contain. 
			{

				strcpy(oparand,token);
				token = strtok(NULL," \t\n\r");
			}
		}
			//creating a Header
			if(strcmp(opcode,"START")==0) //if the opcode is a START from the file then from here the memory address is added to counter for calculation
			{
				char header[20];
				char start_add[7];
				char length[7];
				memset(header,'\0',20*sizeof(char));
				memset(start_add,'\0',7*sizeof(char));
				memset(length,'\0',7*sizeof(char));
				header[0]='H';
				for(int k=0;k<6;k++)
				{
					if(k>=strlen(newsym))
					{
					header[k+1]=' ';
					continue;
					}
					header[k+1]=newsym[k];
				}
				sprintf(start_add,"%06X",start_address);
				//printf("oparand %s\n",oparand);
				for(int k=0; k<6;k++)
				{
					header[k+7]=start_add[k];	
				}
				sprintf(length,"%06X",program_length);
				for(int k=0;k<6;k++)
				{
					header[k+13]=length[k];
				}
				header[19]='\0';
				counter=start_address;
				//call addrecord(hrecord,header);
				Addrecords(Hrecord,header);
				//fprintf(OutputFile,"%s\n",header);
#ifdef DEBUG2
			printf("Testing  %s\n",header);	
#endif		
			
			}
			//here I create a T record for all instruction but has RSUB hardcoded 
			if(checkOpcode(opcode)!=0)
			{
				
				if(strcmp(opcode,"RSUB")!=0)
				{
				//int co=0;
				char Text[70];
				char Op_address[7];
				char opcode_length[3];
				char object_length[58];
				memset(Text,'\0',70*sizeof(char));
				memset(Op_address,'\0',7*sizeof(char));
				memset(opcode_length,'\0',3*sizeof(char));
				memset(object_length,'\0',58*sizeof(char));
				int x_X =0;
				char *space = strchr(oparand,32);
				if(space)
				{
 					 *space = '\0';
				}
				if(strchr(oparand,',') !=NULL)
				{
					token = strtok(oparand,",");
					strcpy(oparand,token);
					char *token3=strtok(NULL,",");
					if(strcmp(token3,"X")==0)
					{
						x_X++;
					}
				}
			
				address_test = Symbol_Address(Symbol_table,oparand);
								
				if(address_test == -1)
				{
					printf("The symbol is not define %s\n",oparand);
					fclose( fp );
					fclose(OutputFile);
					remove(fileName);
					free(newsym);
					free(opcode);
					free(oparand);
					free(token_2);
					free(fileName);
					for(int ab=0;Symbol_table[ab];ab++)
					{
					free(Symbol_table[ab]);
					}
					for(int ab=0;Hrecord[ab];ab++)
					{
					free(Hrecord[ab]);
					}
					for(int ab=0;Trecord[ab];ab++)
					{
					free(Trecord[ab]);
					}
					for(int ab=0;Mrecord[ab];ab++)
					{
					free(Mrecord[ab]);
					}
					for(int ab=0;Endrecord[ab];ab++)
					{
					free(Endrecord[ab]);
					}
					return 0;
				}
				if(x_X==1)
				{
					address_test = address_test+32768;
				}
				Text[0]='T';
				sprintf(Op_address,"%06X",counter);
				char *opcode_value = findOpcodeValue(opcode);
				sprintf(object_length,"%04X",address_test);
				for(int k=0;k<6;k++)
				{
					Text[k+1]=Op_address[k];
				}
				Text[7] ='0';
				Text[8] = '3';
				for(int k=0;k<2;k++)
				{
					opcode_length[k]=opcode_value[k];
					
				}
				
				for(int k=0;k<2;k++)
				{
					Text[k+9]=opcode_length[k];
				}
				for(int k=0;k<6;k++)
				{
					Text[k+11]=object_length[k];
					
				}
				Text[16]='\0';
				//printf("this is for the test %s\t %s\n",opcode,Text);
				Addrecords(Trecord,Text);
				//fprintf(OutputFile,"%s\n",Text);
				}
				if(strcmp(opcode,"RSUB")==0)
				{
					char Text[8];
					memset(Text,'\0',8*sizeof(char));
					Text[0]='T';
					Text[1]='4';
					Text[2]='C';
					Text[3]='0';
					Text[4]='0';
					Text[5]='0';
					Text[6]='0';
					Text[7]='\0';
					Addrecords(Trecord,Text);
					//fprintf(OutputFile,"%s\n",Text);
					//printf("there is a RESUB %s\n",opcode);
				}
				else
				{
				char Modification[17];
				char Address_Modification[7];
				memset(Modification,'\0',17*sizeof(char));
				memset(Address_Modification,'\0',17*sizeof(char));
				Modification[0]='M';
				int modification_address;
				modification_address = counter;
				modification_address +=1;
				address_test +=1;
				sprintf(Address_Modification,"%06X",modification_address);
				for(int k=0;k<6;k++)
				{
					Modification[k+1]=Address_Modification[k];
				} 
				Modification[7]='0';
				Modification[8]='4';
				Modification[9]='+';
				char *start_address_test;
				start_address_test=Symbol_table[0]->Name;
				for(int k=0;k<6;k++)
				{

					Modification[k+10]=start_address_test[k];
				}
				Modification[17]='\0';
				Addrecords(Mrecord,Modification);
				//fprintf(OutputFile,"%s\n",Modification);
				}
			}
			else if(IsADirective(opcode)==1) // from here I create a T record for directive 
			{
				if(strcmp(opcode,"WORD")==0)
				{	
					char Text[70];
					char Op_address[7];
					char object_length[60];
					memset(Text,'\0',70*sizeof(char));
					memset(Op_address,'\0',7*sizeof(char));
					memset(object_length,'\0',58*sizeof(char));

					int address_word =0 ;
					address_word =atoi(oparand);
					Text[0]='T';
					sprintf(Op_address,"%06X",counter);
					for(int k = 0; k<6; k++)
					{
						Text[k+1] = Op_address[k];
					}
					Text[7] ='0';
					Text[8] = '3';
					sprintf(object_length,"%06X",address_word);
					for(int k=0;k<6;k++)
					{
						Text[k+9] = object_length[k];
					}
					Text[15] = '\0';
					Addrecords(Trecord,Text);
					//fprintf(OutputFile,"%s\n",Text);
				}
				
				else if(strcmp(opcode,"BYTE")==0)
				{
					if(oparand[0]==67) // the 67 is ASCII number for C
					{
					int c = 2;
					while(oparand[c]!='\'') // this counts all the bytes in C between two quotes 
					{
						c++;
					}
					c -=3;
					char C_string[c+1];
					memset(C_string,'\0',(c+1)*sizeof(char));
					int ba=2;
					while(oparand[ba] !='\'')
					{
						C_string[ba-2] = oparand[ba];
						ba++;
					}
					
					C_string[ba-2]='\0';
					int c_length=strlen(C_string);
					int split_string=0;
					if(c_length>30)
					{
						split_string=1;
					}
		
					if(split_string==0)
					{
					char Text[70];
					char Op_address[7];
					char length[3];
					memset(Text,'\0',70*sizeof(char));
					memset(Op_address,'\0',7*sizeof(char));
					memset(length,'\0',3*sizeof(char));
					

					char object_length[2*c_length];
					memset(object_length,'\0',(c_length+1)*sizeof(char));
					int Text_length_count =0;
					Text[0]='T';
					sprintf(Op_address,"%06X",counter);
					
					for(int k = 0; k<6; k++)
					{
						Text[k+1] = Op_address[k];
					}
					sprintf(length,"%02X",c_length);
					for(int k = 0; k<2; k++)
					{
						Text[k+7] = length[k];
					}
							
					if(strlen(C_string)>1)
					{
					for(int k=0, l=0;k<strlen(C_string);k++,l+=2)
					{
						
						sprintf(&object_length[l],"%02X",C_string[k]);
					}
					}
					else
					{
						sprintf(object_length,"%02X",C_string[0]);
					}	
					Text_length_count = strlen(object_length) + 9;
					int abbba=strlen(object_length);
					for(int k=0;k<abbba;k++)
					{
						Text[k+9]=object_length[k];
					}
					Text[Text_length_count] = '\0';
					Addrecords(Trecord,Text);
					//fprintf(OutputFile,"%s\n",Text);

					}
					else
					{
						char Text[70];
						char Op_address[7];
						char length[3];
						char object_length[60];
						
					memset(Text,'\0',70*sizeof(char));
					memset(Op_address,'\0',7*sizeof(char));
					memset(length,'\0',3*sizeof(char));
					memset(object_length,'\0',60*sizeof(char));



						int Text_length_count=0;
						int new_record=0;
						if(c_length>30)
						{
							new_record=1;
							Text_length_count=30;							
						}
						Text[0]='T';
						sprintf(Op_address,"%06X",counter);
					
						for(int k = 0; k<6; k++)
						{
						Text[k+1] = Op_address[k];
						}
						sprintf(length,"%02X",Text_length_count);
						for(int k = 0; k<2; k++)
						{
							Text[k+7] = length[k];
						}
						
						for(int k=0, l=0;k<30;k++,l+=2)
						{
						
						sprintf(&object_length[l],"%02X",C_string[k]);
						}
							
					
						for(int k=0;k<60;k++)
						{
							Text[k+9]=object_length[k];
						}
						Text[70] = '\0';
						Addrecords(Trecord,Text);
						//fprintf(OutputFile,"%s\n",Text);
						
						if(new_record==1)
						{
							int for_array = 0;
							for_array=c_length-30;
							char Text[70];
							char Op_address[7];
							char length[3];
							char object_length[2*for_array];
							
							memset(Text,'\0',70*sizeof(char));
							memset(Op_address,'\0',7*sizeof(char));
							memset(length,'\0',3*sizeof(char));
							memset(object_length,'\0',for_array*sizeof(char));
							
							int Text_length_count_1 =0;
							int update_counter = 0;
							update_counter = counter;
							update_counter +=30;
						
							Text[0]='T';
							sprintf(Op_address,"%06X",update_counter);
					
							for(int k = 0; k<6; k++)
							{
								Text[k+1] = Op_address[k];
							}
							sprintf(length,"%02X",Text_length_count);
							for(int k = 0; k<2; k++)
							{
								Text[k+7] = length[k];
							}	
						
							for(int k=0, l=0;k<for_array;k++,l+=2)
							{
						
							sprintf(&object_length[l],"%02X",C_string[k+30]);
							}
							
					
							Text_length_count_1 = strlen(object_length) + 9;
							for(int k=0;k<60;k++)
							{
								Text[k+9]=object_length[k];
							}
							Text[Text_length_count_1] = '\0';
							Addrecords(Trecord,Text);
							//fprintf(OutputFile,"%s\n",Text);

						}

					
					}
					
				}
					else if(oparand[0]==88)
				{
						int c = 2;
					while(oparand[c]!='\'') // this counts all the bytes in C between two quotes 
					{
						c++;
					}
					c -=3;
					char C_string[c+1];
					memset(C_string,'\0',(c+1)*sizeof(char));
					int ba=2;
					while(oparand[ba] !='\'')
					{
						C_string[ba-2] = oparand[ba];
						ba++;
					}
					C_string[ba-2]='\0';
					int c_length= 0; 
					c_length=strlen(C_string);
					int x_length = 0;
					x_length = strlen(C_string)/2;
					
					char Text[70];
					char counter_address[7];
					char length_X[3];
					char object_code_X[c_length];

					memset(Text,'\0',70*sizeof(char));
					memset(counter_address,'\0',7*sizeof(char));
					memset(length_X,'\0',3*sizeof(char));
					memset(object_code_X,'\0',c_length*sizeof(char));
					
					Text[0]='T';
					sprintf(counter_address,"%06X",counter);
					for(int k = 0;k<6;k++)
					{
						Text[k+1]=counter_address[k];
					}
					sprintf(length_X,"%02X",x_length);
					for(int k = 0;k<2;k++)
					{
						Text[k+7]=length_X[k];
					}
					
				
					for(int k = 0;k<c_length;k++)
					{
						object_code_X[k]=C_string[k];	
					}
					int forthetespurpose=strlen(object_code_X);
					for(int k = 0;k<forthetespurpose;k++)
					{
						Text[k+9]=object_code_X[k];
					}
					int anser = 0;	
					anser=strlen(object_code_X) +9;
					Text[anser]='\0';
					Addrecords(Trecord,Text);
					//fprintf(OutputFile,"%s\n",Text);
					
					}
				}
				else if(strcmp(opcode,"END")==0)
				{
					address_test = Symbol_Address(Symbol_table,oparand);			
				if(address_test == -1)
				{
					printf("The symbol is not define %s\n",oparand);
					fclose( fp );
					fclose(OutputFile);
					remove(fileName);
					free(newsym);
					free(opcode);
					free(oparand);
					free(token_2);
					free(fileName);
					for(int ab=0;Symbol_table[ab];ab++)
					{
					free(Symbol_table[ab]);
					}
					for(int ab=0;Hrecord[ab];ab++)
					{
					free(Hrecord[ab]);
					}
					for(int ab=0;Trecord[ab];ab++)
					{
					free(Trecord[ab]);
					}
					for(int ab=0;Mrecord[ab];ab++)
					{
					free(Mrecord[ab]);
					}
					for(int ab=0;Endrecord[ab];ab++)
					{
					free(Endrecord[ab]);
					}
					return 0;
				}
					char End[8];
					char Starting_address[7];

					memset(End,'\0',8*sizeof(char));
					memset(Starting_address,'\0',7*sizeof(char));

					End[0]='E';
					int starting_address_end=first_instruction_int;
					sprintf(Starting_address,"%06X",starting_address_end);
					for(int k=0;k<6;k++)
					{
						End[k+1]=Starting_address[k];
					}
					End[8]='\0';
					Addrecords(Endrecord,End);
					//fprintf(OutputFile,"%s\n",End);

				}
				
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
					//printf("Long Byte Constant is %s\n",oparand);
				}
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
	file_output(OutputFile,Hrecord);
	file_output(OutputFile,Trecord);
	file_output(OutputFile,Mrecord);
	file_output(OutputFile,Endrecord);
	
	free(newsym);
	free(opcode);
	free(oparand);
	free(token_2);
	free(fileName);
	for(int ab=0;Symbol_table[ab];ab++)
	{
		free(Symbol_table[ab]);
	}
	for(int ab=0;Hrecord[ab];ab++)
	{
		free(Hrecord[ab]);
	}
	for(int ab=0;Trecord[ab];ab++)
	{
		free(Trecord[ab]);
	}
	for(int ab=0;Mrecord[ab];ab++)
	{
		free(Mrecord[ab]);
	}
	for(int ab=0;Endrecord[ab];ab++)
	{
		free(Endrecord[ab]);
	}
	
	fclose( fp );
	
	fclose(OutputFile);
	return 0;
	
}

// this checks is given token is oparand or not. if the given token is oparand then it returns 1 else 0
int checkOpcode(char *test)
{
	for(int i=0;i<59;i++)
	{
		if(strcmp(test,Opcode_Table[i].Name)==0)
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
int Symbol_Address(SYMBOL *Tab[],char *Sname)
{
	int index =0;
	int result;
	result=-1;
	while(Tab[index]!=NULL )
	{	//printf("this is in the symbol address |%s|\n",Tab[index]->Name);
		if(strcmp(Tab[index]->Name,Sname) ==0)
		{
			result =Tab[index]->Address;
			//printf("thie is in symbol address %X       %s\n ",result,Sname);
			return result;
			break;
		}
		index++;
	}
	return result;
}

// here all the new symbols are added to symbol table with their address, sorce line and name.
void AddSymbols(SYMBOL *Tab[],int Addr,int Srcline,char *Sname)
{
	int index1 =0;
	SYMBOL *temp;
	temp = malloc(sizeof(SYMBOL));
	temp->Address=Addr;
	temp->DefinedOnSourceLine=Srcline;
	strcpy(temp->Name,Sname);
	while(Tab[index1]!=NULL)
	{
		index1++;
	}
	Tab[index1]=temp;
	
}
void Addrecords(char *records[], char *record_data)
{
	int index2 =0;
	char *temp;
	int check_size=strlen(record_data);
	temp = malloc(check_size*sizeof(record_data));
	memset(temp,'\0',check_size*sizeof(record_data));
	strcpy(temp,record_data);
	while(records[index2]!=NULL)
	{
		index2++;
	}

	records[index2]=temp;
	//printf("this is in the add record %s\n",temp);
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
// I find the opcode and the ther values 
char *findOpcodeValue(char *opcodess){
	for(int i = 0; i < 59; i++){
		if(strcmp(Opcode_Table[i].Name, opcodess) == 0){
			return Opcode_Table[i].opCode;
		}
	}
	return NULL;
}

// write everyting to the output file
void file_output(FILE* Output_in, char *Output_write[])
{
	int index_test_index =0;
	while(Output_write[index_test_index]!=NULL)
	{
		//printf("this in in the file output %s\n",Output_write[index_test_index]);
		fprintf(Output_in,"%s\n",Output_write[index_test_index]);
		index_test_index++;
	}	
}


