#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#define DEBUG 1
//#define DEBUG_SYMBOL 1
//#define DEBUG_OPCODE 1
//#define DEBUG_OPARAND 1
//#define DEBUG2 1

struct symbols
{
	int DefinedOnSourceLine;
	int Address;
	char Name[7];
};

typedef struct symbols SYMBOL;

struct opcodes
{
	char opCode[3];
	char *Name;
};

struct rstable
{
	char value_of_rs[1];
	char *Name;
};

struct locStored
{
	int Fline;
	int locCtr;
};

typedef struct locStored LOCSTORED;
typedef struct rstable RS;
typedef struct opcodes OPCODES;
OPCODES OpcodeTable[32];
int IsAValidSymbol(char *TestSymbol);
int IsADirective(char *Test);
int checkOpcode(char *Opcode);
int check_for_hex(char hex[]);
int Symbol_Exists(SYMBOL *Tab[], char *Sname);
void AddSymbols(SYMBOL *Tab[], int Addr, int Srcline, char *Sname);
void Addrecords(char *records_add[], char *record_datas);
void file_output(FILE *Output_in, char *Output_write[]);
void print_table(SYMBOL *Tab[]);
char *findOpcodeValue(char *opcode);
char *findRegisterValues(char *oparands);
void formatOne(char *ope, char *T_re[], int locpass);
void formatTwo(char *ope, char *opa, char *thirdtoken, char *T_re[], int locpass);
void formatThree(bool hash_symbol, bool at_symbol, bool p_f, bool b_f, bool s_int, char *opcode, char *oparand, char *X_ch, char *T_re[], int fc, int locpass);
void formatFour(bool hash_symbol, bool at_symbol, bool p_f, bool b_f, char *opcode, char *oparand, char *X_ch, char *T_re[], int fc, int locpass);
int Symbol_Address(SYMBOL *Tab[], char *Sname);
void Addloc(struct locStored *Tab[], int current, int Fline);
int getLocCtr(struct locStored *Tab[], int Fline);