#include <stdio.h>
#include <stdlib.h>

//#define DEBUG 1
//#define DEBUG_SYMBOL 1
//#define DEBUG_OPCODE 1
//#define DEBUG_OPARAND 1

struct symbols {
	int	DefinedOnSourceLine;
	int	Address; 
	char	Name[7]; };

typedef struct symbols	SYMBOL;

struct opcodes
{
	char opCode;
	char Name[8];
	
};

typedef struct opcodes OPCODES;
OPCODES OpcodeTable[32];
int IsAValidSymbol( char *TestSymbol );
int IsADirective( char *Test );
int checkOpcode(char* Opcode);
int check_for_hex(char hex[]);
int Symbol_Exit(SYMBOL *Tab[],char *Sname);
void AddSymbols(SYMBOL *Tab[],int Addr,int Srcline,char *Sname);
void print_table(SYMBOL *Tab[]);
