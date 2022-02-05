#include <stdio.h>
#include <stdlib.h>

//#define DEBUG 1
//#define DEBUG_SYMBOL 1
//#define DEBUG_OPCODE 1
//#define DEBUG_OPARAND 1
//#define DEBUG2 1

struct symbols {
	int	DefinedOnSourceLine;
	int	Address; 
	char	Name[7]; };

typedef struct symbols	SYMBOL;

struct opcodes
{
	char opCode[3];
	char *Name;
	
};

typedef struct opcodes OPCODES;
OPCODES OpcodeTable[32];
int IsAValidSymbol( char *TestSymbol );
int IsADirective( char *Test );
int checkOpcode(char* Opcode);
int check_for_hex(char hex[]);
int Symbol_Exit(SYMBOL *Tab[],char *Sname);
void AddSymbols(SYMBOL *Tab[],int Addr,int Srcline,char *Sname);
void Addrecords(char *records_add[], char *record_datas);
void file_output(FILE *Output_in, char *Output_write[]);
void print_table(SYMBOL *Tab[]);
char *findOpcodeValue(char *opcode);
int Symbol_Address(SYMBOL *Tab[],char *Sname);