#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

#define MAX_MEMORY 1048576 // this is the max memory for SIC 2^20
#define MAX_WORD 8388608	 // this is the max word count we can have in SIC 2^24
/**#define A 0
#define X 1
#define L 2
#define B 3
#define S 4
#define T 5
#define F 1**/

// here i have created a static array all the instruction for the SIC.
static OPCODES Opcode_Table[] =
		{
				{"18", "ADD"}, {"58", "ADDF"}, {"90", "ADDR"}, {"40", "AND"}, {"B4", "CLEAR"}, {"28", "COMP"}, {"88", "COMPF"}, {"A0", "COMPR"}, {"24", "DIV"}, {"64", "DIVF"}, {"9C", "DIVR"}, {"C4", "FIX"}, {"C0", "FLOAT"}, {"F4", "HIO"}, {"3C", "J"}, {"30", "JEQ"}, {"34", "JGT"}, {"38", "JLT"}, {"48", "JSUB"}, {"00", "LDA"}, {"68", "LDB"}, {"78", "STB"}, {"70", "LDF"}, {"08", "LDL"}, {"6C", "LDS"}, {"74", "LDT"}, {"04", "LDX"}, {"D0", "LPS"}, {"20", "MUL"}, {"60", "MULF"}, {"98", "MULR"}, {"C8", "NORM"}, {"44", "OR"}, {"D8", "RD"}, {"AC", "RMO"}, {"4C", "RSUB"}, {"A4", "SHIFTL"}, {"A8", "SHIFTR"}, {"F0", "SIO"}, {"EC", "SSK"}, {"0C", "STA"}, {"54", "STCH"}, {"80", "STF"}, {"D4", "STI"}, {"14", "STL"}, {"7C", "STS"}, {"E8", "STSW"}, {"84", "STT"}, {"10", "STX"}, {"1C", "SUB"}, {"5C", "SUBF"}, {"94", "SUBR"}, {"B0", "SVC"}, {"E0", "TD"}, {"F8", "TIO"}, {"2C", "TIX"}, {"B8", "TIXR"}, {"DC", "WD"}, {"50", "LDCH"}};

static RS Rs_table[] =
		{
				{"0", "A"}, {"1", "X"}, {"2", "L"}, {"2", "B"}, {"4", "S"}, {"5", "T"}, {"1", "F"}};

int main(int argc, char *argv[])
{

	FILE *fp;				 // this reads the file name
	char line[1024]; // this is to through line by line from file
	int counter;		 // this count for memory from the file
	int lines = 0;	 // this keep tracks of how many lines are been read on file and where the error is from the file
	char *newsym;		 // the stores new symbol from the file
	char *opcode;		 // this stores opcode from the file
	char *oparand;	 // this stores the memory address from the file
	char *token;		 // this goes through each token and stores them into proper place
	char *token_2;
	char *base;
	char *X_bit;
	int start_address;
	char *Hrecord[1024];
	char *Trecord[1024];
	char *Mrecord[1024];
	char *Endrecord[1024];
	bool hasStartDirective = false;
	int firstInstructionLoc = -1;
	bool isFirstInstruction = false;
	bool hasPlusSymbol = false;
	bool hasHashSymbol = false;
	bool hasAtSymbol = false;
	bool isSecondFormat = false;
	bool isValidInt = false;
	bool p_flag = false;
	bool b_flag = false;

	char fullline[1024]; // this stores the full lines from file
	struct locStored *LocTab[1024];

	if (argc != 2)
	{
		printf("ERROR: Usage: %s filename\n", argv[0]);
		return 0;
	}

	fp = fopen(argv[1], "r");

	if (fp == NULL)
	{
		printf("ERROR: %s could not be opened for reading,\n", argv[1]);
		return 0;
	}

	// here setting all the memory to zero in order to make sure we dont run into segment fault error
	// malloc creates the sizes for the char and memset NULL out everything.
	newsym = malloc(1024 * sizeof(char));
	memset(newsym, '\0', 1024 * sizeof(char));

	opcode = malloc(1024 * sizeof(char));
	memset(opcode, '\0', 1024 * sizeof(char));

	oparand = malloc(1024 * sizeof(char));
	memset(oparand, '\0', 1024 * sizeof(char));

	token_2 = token = malloc(1024 * sizeof(char));
	memset(token, '\0', 1024 * sizeof(char));

	base = malloc(1024 * sizeof(char));
	memset(base, '\0', 1024 * sizeof(char));

	X_bit = malloc(1024 * sizeof(char));
	memset(X_bit, '\0', 1024 * sizeof(char));

	memset(Hrecord, '\0', 1024 * sizeof(char *));
	memset(Trecord, '\0', 1024 * sizeof(char *));
	memset(Mrecord, '\0', 1024 * sizeof(char *));
	memset(Endrecord, '\0', 1024 * sizeof(char *));
	memset(LocTab, '\0', 1024 * sizeof(struct locStored *));

	// creating a symbol table array for the pointer to create a symbol table.
	SYMBOL *Symbol_table[1024];
	memset(Symbol_table, '\0', 1024 * sizeof(SYMBOL *));

	// in this while loop we go through line by line and see what we need to do with that line
	while (fgets(line, 1024, fp) != NULL)
	{
		lines++;
		strcpy(fullline, line); // each line that currenctly is beign read is get coppied to fullline for storage in order to print it out for the error.
		if (line[0] == 35)
		{ // the 35 is an ASCII number for #
			continue;
		}
		if ((line[0] >= 65) && (line[0] <= 90)) // the 65 and 90 is an ASCII number for alphabate A to Z
		{
			token = strtok(line, " \t\n\r"); // the first token is read
			if (IsAValidSymbol(token) == 0)	 // checking for error
			{
				printf("ERROR. INVALID SYMBOL\n");
				fclose(fp);
				free(newsym);
				free(opcode);
				free(oparand);
				free(base);
				free(token_2);
				for (int ab = 0; Symbol_table[ab]; ab++)
				{
					free(Symbol_table[ab]);
				}
				for (int ab = 0; LocTab[ab]; ab++)
				{
					free(LocTab[ab]);
				}
				return 0;
			}
			else
			{
				strcpy(newsym, token);					 // the first token is store in symbol table
				token = strtok(NULL, " \t\n\r"); // this moves to next token
			}
			if (IsADirective(newsym) == 1) // if the symbol is a directive then it thorugh an error
			{
				printf("Testing case %s SIC Program Has Symbol Name that is an assembler Directive %s\n", argv[1], newsym);
				printf("It is on line %d and line is %s", lines, fullline);
				fclose(fp);
				free(newsym);
				free(opcode);
				free(oparand);
				free(base);
				free(token_2);
				for (int ab = 0; Symbol_table[ab]; ab++)
				{
					free(Symbol_table[ab]);
				}
				for (int ab = 0; LocTab[ab]; ab++)
				{
					free(LocTab[ab]);
				}
				return 0;
			}
			if (checkOpcode(newsym) == 1) // if the symbol is an instruction then it thorugh an error
			{
				printf("Testing case %s SIC Program Has Symbol Name that is an assembler Instruction %s\n", argv[1], newsym);
				printf("It is on line %d and line is %s", lines, fullline);
				fclose(fp);
				free(newsym);
				free(opcode);
				free(oparand);
				free(base);
				free(token_2);
				for (int ab = 0; Symbol_table[ab]; ab++)
				{
					free(Symbol_table[ab]);
				}
				for (int ab = 0; LocTab[ab]; ab++)
				{
					free(LocTab[ab]);
				}
				return 0;
			}

			if (token[0] == 43) // this check if the first charactor is a plus sign if it is, it makes bool true for counter to add 4
			{
				hasPlusSymbol = true;
				token++;
			}
			else // if there is not a plus sign then it makes boolean false and its does regular counter update
			{
				hasPlusSymbol = false;
			}
			if (IsADirective(token) == 0) // now it check if the next token is directive or instruction if it is neither than it through an error
			{
				if (checkOpcode(token) == 0)
				{
					printf("this is not a token %s\n", token);
					fclose(fp);
					free(newsym);
					free(opcode);
					free(oparand);
					free(base);
					free(token_2);
					for (int ab = 0; Symbol_table[ab]; ab++)
					{
						free(Symbol_table[ab]);
					}
					for (int ab = 0; LocTab[ab]; ab++)
					{
						free(LocTab[ab]);
					}
					return 0;
				}
			}

			strcpy(opcode, token); // if the token passes the error test than it is copied to opcode

			token = strtok(NULL, "\t\n\r");						// this moves to next token and also i have removed the delimeter space because it was causing an error while reading address
			if ((token[0] == 35) || (token[0] == 64)) // this checks if first charactor is #,@. if so it moves token by one
			{
				token++;
			}
			strcpy(oparand, token); // this copies memory address into oparand

			token = strtok(NULL, " \t\n\r");
			if (strcmp(opcode, "START") == 0) // if the opcode is a START from the file then from here the memory address is added to counter for calculation
			{
				hasStartDirective = true;
				counter = strtol(oparand, NULL, 16); // strtol converts the string into the long integer and it is assigned to counter
				start_address = counter;

				if (counter == MAX_MEMORY) // here i am checking if the memory where it start is already at max if so it through an error and stops the code
				{
					printf("Testing case %s SIC Program Starts at Hex %04X No room left in SIC memory.\n", argv[1], counter);
					fclose(fp);
					free(newsym);
					free(opcode);
					free(oparand);
					free(base);
					free(token_2);
					for (int ab = 0; Symbol_table[ab]; ab++)
					{
						free(Symbol_table[ab]);
					}
					for (int ab = 0; LocTab[ab]; ab++)
					{
						free(LocTab[ab]);
					}
					return 0;
				}
			}
			if (Symbol_Exists(Symbol_table, newsym) == -1) // if the symbol already exits in symbol table then it through an error
			{
				printf("Test Case %s SIC Program Has %s defind twice\n", argv[1], newsym);
				printf("It is on line %d and line is %s", lines, fullline);
				fclose(fp);
				free(newsym);
				free(opcode);
				free(oparand);
				free(base);
				free(token_2);
				for (int ab = 0; Symbol_table[ab]; ab++)
				{
					free(Symbol_table[ab]);
				}
				for (int ab = 0; LocTab[ab]; ab++)
				{
					free(LocTab[ab]);
				}
				return 0;
			}
			else
			{
				AddSymbols(Symbol_table, counter, lines, newsym); // if symbol doesnt exit in symbol table then it is added
			}
		}

		else if (line[0] == 9 || line[0] == 32) // the 9 is an ASCII number for tab if the symbol contains tab then it stright goes to oparand.
		{
			token = strtok(line, " \t\n\r");
			if (token[0] == 43) // checks if the char is a +
			{
				hasPlusSymbol = true;
				token++;
			}
			else
			{
				hasPlusSymbol = false;
			}
			if (IsADirective(token) == 0)
			{
				if (checkOpcode(token) == 0)
				{
					printf("this is not a token 137 %s\n", token);
					fclose(fp);
					free(newsym);
					free(X_bit);
					free(opcode);
					free(oparand);
					free(base);
					free(token_2);
					for (int ab = 0; Symbol_table[ab]; ab++)
					{
						free(Symbol_table[ab]);
					}
					for (int ab = 0; LocTab[ab]; ab++)
					{
						free(LocTab[ab]);
					}
					return 0;
				}
			}
			strcpy(opcode, token);
			token = strtok(NULL, "\t\n\r");
			if (token != NULL)
			{
				if ((token[0] == 35) || (token[0] == 64)) // this checks if first charactor is #,@. if so it moves token by one
				{
					token++;
				}
			}
			while (token != NULL) // which copies full memory address to oparand when there is a space contain.
			{
				strcpy(oparand, token);
				token = strtok(NULL, " \t\n\r");
			}
		}
		else // this check if the first symbol has any invilid symbol or if the file contains blank space in the file. if so then it through an error.
		{
			for (int i = 0; i < strlen(line); i++)
			{
				if ((line[i] == 36) || (line[i] == 33) || (line[i] == 37) || (line[i] == 61) || (line[i] == 43) || (line[i] == 45) || (line[i] == 40) || (line[i] == 41) || (line[i] == 64))
				{
					printf("Testing case %s Program Has Invalid Symbol Name\n", argv[1]);
					printf("Full Invilid symbol is on line %d line is %s", lines, fullline);
					fclose(fp);
					free(newsym);
					free(opcode);
					free(oparand);
					free(base);
					free(token_2);
					for (int ab = 0; Symbol_table[ab]; ab++)
					{
						free(Symbol_table[ab]);
					}
					for (int ab = 0; LocTab[ab]; ab++)
					{
						free(LocTab[ab]);
					}
					return 0;
				}
			}
			printf("Testing case %s Has blank lines on line %d\n", argv[1], lines);
			fclose(fp);
			free(newsym);
			free(opcode);
			free(oparand);
			free(base);
			free(token_2);
			for (int ab = 0; Symbol_table[ab]; ab++)
			{
				free(Symbol_table[ab]);
			}
			for (int ab = 0; LocTab[ab]; ab++)
			{
				free(LocTab[ab]);
			}
			return 0;
		}

		if (counter > MAX_MEMORY) // checking if the memory is bigger than max memeory if so then it through an error.
		{
			if (!hasStartDirective)
			{
				printf("Testing case %s Program Does not fit in SIC Memory, Memory is too big %04X\n", argv[1], counter);
				fclose(fp);
				free(newsym);
				free(opcode);
				free(oparand);
				free(base);
				free(X_bit);
				free(token_2);

				for (int ab = 0; Symbol_table[ab]; ab++)
				{
					free(Symbol_table[ab]);
				}
				for (int ab = 0; LocTab[ab]; ab++)
				{
					free(LocTab[ab]);
				}
				return 0;
			}
		}
		if (strcmp(opcode, "BASE") == 0)
		{
			strcpy(base, oparand);
		}
		else
		{
			if (strcmp(opcode, "END") != 0) // this check if the opcode is equal to END if so then this all calculation stops.
			{
				// Addloc(LocTab, counter, lines);
				if (checkOpcode(opcode) == 1) // if the given opcode is equall to oparand then the counter is incresed by 3
				{
					Addloc(LocTab, counter, lines);
					if (isFirstInstruction == false)
					{
						firstInstructionLoc = counter;
						isFirstInstruction = true;
					}
					if (hasPlusSymbol == true)
					{
						counter += 4;
					}
					else if (hasPlusSymbol == false)
					{
						if (strcmp(opcode, "ADDR") == 0 || strcmp(opcode, "CLEAR") == 0 || strcmp(opcode, "COMPR") == 0 || strcmp(opcode, "DIVR") == 0 || strcmp(opcode, "MULR") == 0 || strcmp(opcode, "RMO") == 0 || strcmp(opcode, "SHIFTL") == 0 || strcmp(opcode, "SHIFTR") == 0 || strcmp(opcode, "SUBR") == 0 || strcmp(opcode, "SVC") == 0 || strcmp(opcode, "TIXR") == 0)
						{
							counter += 2;
						}
						else if (strcmp(opcode, "FIX") == 0 || strcmp(opcode, "FLOAT") == 0 || strcmp(opcode, "HIO") == 0 || strcmp(opcode, "NORM") == 0 || strcmp(opcode, "SIO") == 0 || strcmp(opcode, "TIO") == 0)
						{
							counter += 1;
						}
						else
						{
							counter += 3;
						}
					}
				}
				else if (strcmp(opcode, "BYTE") == 0) // this checks if the opcode is byte. then it check if the address starts with C or X
				{
					Addloc(LocTab, counter, lines);

					if (oparand[0] == 67) // the 67 is ASCII number for C
					{
						int c = 2;
						while (oparand[c++] != '\'')
							;			// this counts all the bytes in C between two quotes
						c -= 3; // dont count "C''"
						// printf("this is for the C %d\n",c);
						counter += c;
					}
					else if (oparand[0] == 88) // the 88 is ASCII for X
					{
						int x = 2;
						char hex[10]; // this sotes everything from X between two quotes
						while (oparand[x] != '\'')
						{
							hex[x - 2] = oparand[x];
							x++;
						}
						hex[x - 2] = '\0';
						// printf("%d hex return\n",check_for_hex(hex));
						if (check_for_hex(hex) == 0) // this check for invild hex digit.
						{
							printf("Testing case %s SIC Program Has Invalid Hex Constant\n", argv[1]);
							printf("The Invilid hex on line %d is %s", lines, fullline);
							fclose(fp);
							free(newsym);
							free(opcode);
							free(oparand);
							free(X_bit);
							free(base);
							free(token_2);
							for (int ab = 0; Symbol_table[ab]; ab++)
							{
								free(Symbol_table[ab]);
							}
							for (int ab = 0; LocTab[ab]; ab++)
							{
								free(LocTab[ab]);
							}
							return 0;
						}
						else
						{
							int X_bytelength = strlen(hex);
							// printf("line 196 %d\n",X);
							counter += (X_bytelength / 2);
						}
					}
				}
				else if (strcmp(opcode, "WORD") == 0) // this check if the opcode is word or not.
				{
					int oparand_int = atoi(oparand); // here it converts string to integer in order to check for the error
					if (oparand_int >= MAX_WORD)
					{
						printf("Testing case %s WORD Contant Exceeds 24 bit limitation\n", argv[1]);
						fclose(fp);
						free(newsym);
						free(opcode);
						free(X_bit);
						free(oparand);
						free(base);
						free(token_2);
						for (int ab = 0; Symbol_table[ab]; ab++)
						{
							free(Symbol_table[ab]);
						}
						for (int ab = 0; LocTab[ab]; ab++)
						{
							free(LocTab[ab]);
						}
						return 0;
					}
					Addloc(LocTab, counter, lines);
					counter += 3;
				}
				// this check if the opcode is resb. if it is then the memory address is converted from string to integer and added to counter
				else if (strcmp(opcode, "RESB") == 0)

				{
					Addloc(LocTab, counter, lines);
					counter += atoi(oparand);
				}
				// this check if the opcode is resw. if it is then the memory address is converted from string to integer and multipled by 3 then added to counter
				else if (strcmp(opcode, "RESW") == 0)

				{
					Addloc(LocTab, counter, lines);
					counter = counter + (3 * atoi(oparand));
				}
			}
			else
			{
				Addloc(LocTab, counter, lines);
			}
		}
	}
	if (!hasStartDirective) // if theres no start directive
	{
		printf("Testing case %s is missing START direvtive\n", argv[1]);
		fclose(fp);
		free(newsym);
		free(opcode);
		free(oparand);
		free(X_bit);
		free(base);
		free(token_2);
		for (int ab = 0; Symbol_table[ab]; ab++)
		{
			free(Symbol_table[ab]);
		}
		for (int ab = 0; LocTab[ab]; ab++)
		{
			free(LocTab[ab]);
		}
		return 0;
	}

	// print_table(Symbol_table);
	int program_length = 0;
	program_length = counter - start_address;

	rewind(fp);
	hasPlusSymbol = false;
	FILE *OutputFile;
	char *fileName = (char *)malloc((strlen(argv[1]) + 5) * sizeof(char));
	strcpy(fileName, argv[1]);
	strcat(fileName, ".obj");
	OutputFile = fopen(fileName, "w");

	int address_test = 0; // this is so i can do location counting again

	while (fgets(line, 1024, fp) != NULL)
	{
		lines++;
		strcpy(fullline, line); // each line that currenctly is beign read is get coppied to fullline for storage in order to print it out for the error.

		// Gets the location counter at the current line in the file
		if (line[0] == 35)
		{ // the 35 is an ASCII number for #
			continue;
		}
		if ((line[0] >= 65) && (line[0] <= 90)) // the 65 and 90 is an ASCII number for alphabate A to Z
		{
			token = strtok(line, " \t\n\r"); // the first token is read
			strcpy(newsym, token);					 // the first token is store in symbol table
			token = strtok(NULL, " \t\n\r"); // this moves to next token
			if (token[0] == 43)							 // this check if the first charactor is a plus sign if it is, it makes bool true for counter to add 4
			{
				hasPlusSymbol = true;
				token++;
			}
			else // if there is not a plus sign then it makes boolean false and its does regular counter update
			{
				hasPlusSymbol = false;
			}
			strcpy(opcode, token);
			token = strtok(NULL, "\t\n\r");
			if (token[0] == 35) // this checks if first charactor is # if so it moves token by one
			{
				hasHashSymbol = true;
				token++;
			}
			else
			{
				hasHashSymbol = false;
			}
			if (token[0] == 64) // this checks if first charactor is @. if so it moves token by one
			{
				hasAtSymbol = true;
				token++;
			}
			else
			{
				hasAtSymbol = false;
			}

			strcpy(oparand, token);
			token = strtok(NULL, " \t\n\r");
		}

		else if (line[0] == 9 || line[0] == 32) // the 9 is an ASCII number for tab if the symbol contains tab then it stright goes to oparand.
		{

			token = strtok(line, " \t\n\r");
			if (token[0] == 43) // this check if the first charactor is a plus sign if it is, it makes bool true for counter to add 4
			{
				hasPlusSymbol = true;
				token++;
			}
			else // if there is not a plus sign then it makes boolean false and its does regular counter update
			{
				hasPlusSymbol = false;
			}
			strcpy(opcode, token);
			token = strtok(NULL, "\t\n\r");
			if (token != NULL)
			{
				if (token[0] == 35) // this checks if first charactor is #. if so it moves token by one
				{
					hasHashSymbol = true;
					token++;
				}
				else
				{
					hasHashSymbol = false;
				}
				if (token[0] == 64) // this checks if first charactor is @. if so it moves token by one
				{
					hasAtSymbol = true;
					token++;
				}
				else
				{
					hasAtSymbol = false;
				}
			}

			if (token == NULL)
			{
				strcpy(oparand, "NULL");
				token = strtok(NULL, " \t\n\r");
			}
			while (token != NULL) // which copies full memory address to oparand when there is a space contain.
			{
				strcpy(oparand, token);
				token = strtok(NULL, " \t\n\r");
			}
		}
		// creating a Header
		if (strcmp(opcode, "START") == 0) // if the opcode is a START from the file then from here the memory address is added to counter for calculation
		{
			char header[20];
			char start_add[7];
			char length[7];
			memset(header, '\0', 20 * sizeof(char));
			memset(start_add, '\0', 7 * sizeof(char));
			memset(length, '\0', 7 * sizeof(char));
			header[0] = 'H';
			for (int k = 0; k < 6; k++)
			{
				if (k >= strlen(newsym))
				{
					header[k + 1] = ' ';
					continue;
				}
				header[k + 1] = newsym[k];
			}
			sprintf(start_add, "%06X", start_address);
			for (int k = 0; k < 6; k++)
			{
				header[k + 7] = start_add[k];
			}
			sprintf(length, "%06X", program_length);
			for (int k = 0; k < 6; k++)
			{
				header[k + 13] = length[k];
			}
			header[19] = '\0';
			counter = start_address;
			Addrecords(Hrecord, header);
		}
		// printf("this is in before check |%s|\n",opcode);
		if (checkOpcode(opcode) != 0)
		{
			// printf("this is in after check |%s|\n",opcode);
			if (strcmp(opcode, "RSUB") != 0)
			{
				char Text[70];
				char Op_address[7];
				char opcode_length[3];
				char object_length[58];
				memset(Text, '\0', 70 * sizeof(char));
				memset(Op_address, '\0', 7 * sizeof(char));
				memset(opcode_length, '\0', 3 * sizeof(char));
				memset(object_length, '\0', 58 * sizeof(char));
				char *space = strchr(oparand, 32);

				if (space)
				{
					*space = '\0';
				}
				// printf("opcode is %s oparand is %s\n",opcode,oparand);
				if (opcode != NULL && strcmp(oparand, "NULL") == 0)
				{
					formatOne(opcode, Trecord, counter);
				}
				else
				{
					if (strchr(oparand, ',') != NULL)
					{
						token = strtok(oparand, ",");
						strcpy(oparand, token);
						char *token3 = strtok(NULL, ",");
						strcpy(X_bit, token3);
					}
					else
					{
						strcpy(X_bit, "NULL");
					}
					if (strcmp(oparand, "A") == 0 || strcmp(oparand, "X") == 0 || strcmp(oparand, "L") == 0 || strcmp(oparand, "B") == 0 || strcmp(oparand, "S") == 0 || strcmp(oparand, "T") == 0 ||
							strcmp(oparand, "F") == 0)
					{
						formatTwo(opcode, oparand, X_bit, Trecord, counter);
						isSecondFormat = true;
					}
					else
					{
						isSecondFormat = false;
					}
					isValidInt = false;
					// printf("this is in after check |%s|		%d\n",opcode,hasHashSymbol);
					int final_address = 0;
					if (isSecondFormat == false && hasPlusSymbol == false)
					{
						if (hasHashSymbol == true)
						{
							for (int ds = 0; ds < strlen(oparand); ds++)
							{
								if (isdigit(oparand[ds]) != 0)
								{
									isValidInt = true;
								}
								else
								{
									isValidInt = false;
								}
							}
						}
						if (isValidInt == true)
						{
							final_address = atoi(oparand);
							p_flag = false;
							b_flag = false;
						}
						else
						{
							// check if operand is in the sym table
							address_test = Symbol_Address(Symbol_table, oparand);
							if (address_test == -1)
							{
								printf("The symbol is not define %s\n", oparand);
								fclose(fp);
								free(newsym);
								free(opcode);
								free(oparand);
								free(token_2);
								free(X_bit);
								free(base);
								free(fileName);
								for (int ab = 0; Symbol_table[ab]; ab++)
								{
									free(Symbol_table[ab]);
								}
								for (int ab = 0; LocTab[ab]; ab++)
								{
									free(LocTab[ab]);
								}
								for (int ab = 0; Hrecord[ab]; ab++)
								{
									free(Hrecord[ab]);
								}
								for (int ab = 0; Trecord[ab]; ab++)
								{
									free(Trecord[ab]);
								}
								for (int ab = 0; Mrecord[ab]; ab++)
								{
									free(Mrecord[ab]);
								}
								for (int ab = 0; Endrecord[ab]; ab++)
								{
									free(Endrecord[ab]);
								}
								fclose(OutputFile);
								return 0;
							}
							int currentLocCtr = 0;
							currentLocCtr = getLocCtr(LocTab, counter);
							final_address = address_test - currentLocCtr;
							if (final_address >= -2048 && final_address <= 2047)
							{
								p_flag = true;
								b_flag = false;
							}
							else
							{
								int get_base_address = 0;
								address_test = Symbol_Address(Symbol_table, oparand);
								get_base_address = Symbol_Address(Symbol_table, base);
								final_address = address_test - get_base_address;

								if (final_address >= 0 && final_address <= 4095)
								{
									p_flag = false;
									b_flag = true;
								}
								else
								{
									p_flag = false;
									b_flag = false;
									final_address = address_test;
								}
							}
						}
						formatThree(hasHashSymbol, hasAtSymbol, p_flag, b_flag, isValidInt, opcode, oparand, X_bit, Trecord, final_address, counter);
					}
					/******************************************forth formate start here *****************************/
					if (hasPlusSymbol == true)
					{
						if (hasHashSymbol == true)
						{
							for (int ds = 0; ds < strlen(oparand); ds++)
							{
								if (isdigit(oparand[ds]) != 0)
								{
									isValidInt = true;
								}
								else
								{
									isValidInt = false;
								}
							}
						}
						if (isValidInt == true)
						{
							final_address = atoi(oparand);
							p_flag = false;
							b_flag = false;
						}
						else
						{
							address_test = Symbol_Address(Symbol_table, oparand);
							final_address = address_test;
							if (hasAtSymbol == false)
							{
								p_flag = false;
								b_flag = false;
							}
							if (hasAtSymbol == true)
							{
								if ((final_address >= -2048 && final_address <= 2048))
								{
									p_flag = true;
									b_flag = false;
								}
								else
								{
									int get_base_address = 0;
									address_test = Symbol_Address(Symbol_table, oparand);
									get_base_address = Symbol_Address(Symbol_table, base);
									final_address = address_test - get_base_address;

									if (final_address >= 0 && final_address <= 4095)
									{
										p_flag = false;
										b_flag = true;
									}
									else
									{
										p_flag = false;
										b_flag = false;

										if (p_flag == false && b_flag == false)
										{
											printf("This is the wrong address mode\n");
											fclose(fp);
											free(newsym);
											free(opcode);
											free(oparand);
											free(token_2);
											free(X_bit);
											free(base);
											free(fileName);
											for (int ab = 0; Symbol_table[ab]; ab++)
											{
												free(Symbol_table[ab]);
											}
											for (int ab = 0; LocTab[ab]; ab++)
											{
												free(LocTab[ab]);
											}
											for (int ab = 0; Hrecord[ab]; ab++)
											{
												free(Hrecord[ab]);
											}
											for (int ab = 0; Trecord[ab]; ab++)
											{
												free(Trecord[ab]);
											}
											for (int ab = 0; Mrecord[ab]; ab++)
											{
												free(Mrecord[ab]);
											}
											for (int ab = 0; Endrecord[ab]; ab++)
											{
												free(Endrecord[ab]);
											}
											fclose(OutputFile);
											return 0;
										}
									}
								}
							}
						}
						formatFour(hasHashSymbol, hasAtSymbol, p_flag, b_flag, opcode, oparand, X_bit, Trecord, final_address, counter);
					}
				}

				if (hasPlusSymbol == true && hasHashSymbol == false && hasAtSymbol == false)
				{
					char Modification[17];
					char Address_Modification[7];
					memset(Modification, '\0', 17 * sizeof(char));
					memset(Address_Modification, '\0', 17 * sizeof(char));
					Modification[0] = 'M';
					int modification_address = 0;
					modification_address = counter;
					modification_address += 1;
					sprintf(Address_Modification, "%06X", modification_address);
					for (int k = 0; k < 6; k++)
					{
						Modification[k + 1] = Address_Modification[k];
					}
					Modification[7] = '0';
					Modification[8] = '5';
					Modification[9] = '+';
					char *start_address_test;
					start_address_test = Symbol_table[0]->Name;
					for (int k = 0; k < 6; k++)
					{

						Modification[k + 10] = start_address_test[k];
					}
					Modification[17] = '\0';
					Addrecords(Mrecord, Modification);
				}
			}
			if (strcmp(opcode, "RSUB") == 0)
			{
				char Text[8];
				memset(Text, '\0', 8 * sizeof(char));
				Text[0] = 'T';
				Text[1] = '4';
				Text[2] = 'C';
				Text[3] = '0';
				Text[4] = '0';
				Text[5] = '0';
				Text[6] = '0';
				Text[7] = '\0';
				Addrecords(Trecord, Text);
			}
		}

		else if (IsADirective(opcode) == 1) // from here I create a T record for directive
		{
			if (strcmp(opcode, "WORD") == 0)
			{
				char Text[70];
				char Op_address[7];
				char object_length[60];
				memset(Text, '\0', 70 * sizeof(char));
				memset(Op_address, '\0', 7 * sizeof(char));
				memset(object_length, '\0', 58 * sizeof(char));

				int address_word = 0;
				address_word = atoi(oparand);
				Text[0] = 'T';
				sprintf(Op_address, "%06X", counter);
				for (int k = 0; k < 6; k++)
				{
					Text[k + 1] = Op_address[k];
				}
				Text[7] = '0';
				Text[8] = '3';
				sprintf(object_length, "%06X", address_word);
				for (int k = 0; k < 6; k++)
				{
					Text[k + 9] = object_length[k];
				}
				Text[15] = '\0';
				Addrecords(Trecord, Text);
				// fprintf(OutputFile,"%s\n",Text);
			}

			else if (strcmp(opcode, "BYTE") == 0)
			{
				if (oparand[0] == 67) // the 67 is ASCII number for C
				{
					int c = 2;
					while (oparand[c] != '\'') // this counts all the bytes in C between two quotes
					{
						c++;
					}
					c -= 3;
					char C_string[c + 1];
					memset(C_string, '\0', (c + 1) * sizeof(char));
					int ba = 2;
					while (oparand[ba] != '\'')
					{
						C_string[ba - 2] = oparand[ba];
						ba++;
					}

					C_string[ba - 2] = '\0';
					int c_length = strlen(C_string);
					int split_string = 0;
					if (c_length > 30)
					{
						split_string = 1;
					}

					if (split_string == 0)
					{
						char Text[70];
						char Op_address[7];
						char length[3];
						memset(Text, '\0', 70 * sizeof(char));
						memset(Op_address, '\0', 7 * sizeof(char));
						memset(length, '\0', 3 * sizeof(char));

						char object_length[2 * c_length];
						memset(object_length, '\0', (c_length + 1) * sizeof(char));
						int Text_length_count = 0;
						Text[0] = 'T';
						sprintf(Op_address, "%06X", counter);

						for (int k = 0; k < 6; k++)
						{
							Text[k + 1] = Op_address[k];
						}
						sprintf(length, "%02X", c_length);
						for (int k = 0; k < 2; k++)
						{
							Text[k + 7] = length[k];
						}

						if (strlen(C_string) > 1)
						{
							for (int k = 0, l = 0; k < strlen(C_string); k++, l += 2)
							{

								sprintf(&object_length[l], "%02X", C_string[k]);
							}
						}
						else
						{
							sprintf(object_length, "%02X", C_string[0]);
						}
						Text_length_count = strlen(object_length) + 9;
						int abbba = strlen(object_length);
						for (int k = 0; k < abbba; k++)
						{
							Text[k + 9] = object_length[k];
						}
						Text[Text_length_count] = '\0';
						Addrecords(Trecord, Text);
						// fprintf(OutputFile,"%s\n",Text);
					}
					else
					{
						char Text[70];
						char Op_address[7];
						char length[3];
						char object_length[60];

						memset(Text, '\0', 70 * sizeof(char));
						memset(Op_address, '\0', 7 * sizeof(char));
						memset(length, '\0', 3 * sizeof(char));
						memset(object_length, '\0', 60 * sizeof(char));

						int Text_length_count = 0;
						int new_record = 0;
						if (c_length > 30)
						{
							new_record = 1;
							Text_length_count = 30;
						}
						Text[0] = 'T';
						sprintf(Op_address, "%06X", counter);

						for (int k = 0; k < 6; k++)
						{
							Text[k + 1] = Op_address[k];
						}
						sprintf(length, "%02X", Text_length_count);
						for (int k = 0; k < 2; k++)
						{
							Text[k + 7] = length[k];
						}

						for (int k = 0, l = 0; k < 30; k++, l += 2)
						{

							sprintf(&object_length[l], "%02X", C_string[k]);
						}

						for (int k = 0; k < 60; k++)
						{
							Text[k + 9] = object_length[k];
						}
						Text[70] = '\0';
						Addrecords(Trecord, Text);
						// fprintf(OutputFile,"%s\n",Text);

						if (new_record == 1)
						{
							int for_array = 0;
							for_array = c_length - 30;
							char Text[70];
							char Op_address[7];
							char length[3];
							char object_length[2 * for_array];

							memset(Text, '\0', 70 * sizeof(char));
							memset(Op_address, '\0', 7 * sizeof(char));
							memset(length, '\0', 3 * sizeof(char));
							memset(object_length, '\0', for_array * sizeof(char));

							int Text_length_count_1 = 0;
							int update_counter = 0;
							update_counter = counter;
							update_counter += 30;

							Text[0] = 'T';
							sprintf(Op_address, "%06X", update_counter);

							for (int k = 0; k < 6; k++)
							{
								Text[k + 1] = Op_address[k];
							}
							sprintf(length, "%02X", Text_length_count);
							for (int k = 0; k < 2; k++)
							{
								Text[k + 7] = length[k];
							}

							for (int k = 0, l = 0; k < for_array; k++, l += 2)
							{

								sprintf(&object_length[l], "%02X", C_string[k + 30]);
							}

							Text_length_count_1 = strlen(object_length) + 9;
							for (int k = 0; k < 60; k++)
							{
								Text[k + 9] = object_length[k];
							}
							Text[Text_length_count_1] = '\0';
							Addrecords(Trecord, Text);
							// fprintf(OutputFile,"%s\n",Text);
						}
					}
				}
				else if (oparand[0] == 88)
				{
					int c = 2;
					while (oparand[c] != '\'') // this counts all the bytes in C between two quotes
					{
						c++;
					}
					c -= 3;
					char C_string[c + 1];
					memset(C_string, '\0', (c + 1) * sizeof(char));
					int ba = 2;
					while (oparand[ba] != '\'')
					{
						C_string[ba - 2] = oparand[ba];
						ba++;
					}
					C_string[ba - 2] = '\0';
					int c_length = 0;
					c_length = strlen(C_string);
					int x_length = 0;
					x_length = strlen(C_string) / 2;
					char Text[70];
					char counter_address[7];
					char length_X[3];
					char object_code_X[c_length];

					memset(Text, '\0', 70 * sizeof(char));
					memset(counter_address, '\0', 7 * sizeof(char));
					memset(length_X, '\0', 3 * sizeof(char));
					memset(object_code_X, '\0', c_length * sizeof(char));

					Text[0] = 'T';
					sprintf(counter_address, "%06X", counter);
					for (int k = 0; k < 6; k++)
					{
						Text[k + 1] = counter_address[k];
					}
					sprintf(length_X, "%02X", x_length);
					for (int k = 0; k < 2; k++)
					{
						Text[k + 7] = length_X[k];
					}

					for (int k = 0; k < c_length; k++)
					{
						object_code_X[k] = C_string[k];
					}
					int forthetespurpose = 0;
					forthetespurpose = c_length;
					for (int k = 0; k < forthetespurpose; k++)
					{
						Text[k + 9] = object_code_X[k];
					}
					int finaplaceholder = 0;
					finaplaceholder = c_length + 9;
					Text[finaplaceholder] = '\0';
					Addrecords(Trecord, Text);
				}
			}
			else if (strcmp(opcode, "END") == 0)
			{
				address_test = Symbol_Address(Symbol_table, oparand);
				if (address_test == -1)
				{
					printf("The symbol is not define %s\n", oparand);
					fclose(fp);
					free(newsym);
					free(opcode);
					free(oparand);
					free(token_2);
					free(X_bit);
					free(base);
					free(fileName);
					for (int ab = 0; Symbol_table[ab]; ab++)
					{
						free(Symbol_table[ab]);
					}
					for (int ab = 0; LocTab[ab]; ab++)
					{
						free(LocTab[ab]);
					}
					for (int ab = 0; Hrecord[ab]; ab++)
					{
						free(Hrecord[ab]);
					}
					for (int ab = 0; Trecord[ab]; ab++)
					{
						free(Trecord[ab]);
					}
					for (int ab = 0; Mrecord[ab]; ab++)
					{
						free(Mrecord[ab]);
					}
					for (int ab = 0; Endrecord[ab]; ab++)
					{
						free(Endrecord[ab]);
					}
					fclose(OutputFile);
					return 0;
				}
				char End[8];
				char Starting_address[7];

				memset(End, '\0', 8 * sizeof(char));
				memset(Starting_address, '\0', 7 * sizeof(char));

				End[0] = 'E';
				int starting_address_end = firstInstructionLoc;
				sprintf(Starting_address, "%06X", starting_address_end);
				for (int k = 0; k < 6; k++)
				{
					End[k + 1] = Starting_address[k];
				}
				End[8] = '\0';
				Addrecords(Endrecord, End);
				// fprintf(OutputFile,"%s\n",End);
			}
		}

		if (strcmp(opcode, "END") != 0) // this check if the opcode is equall to END if so then this all calculation stops.
		{

			if (checkOpcode(opcode) == 1) // if the given opcode is equall to oparand then the counter is incresed by 3
			{
				if (isFirstInstruction == false)
				{
					firstInstructionLoc = counter;
					isFirstInstruction = true;
				}
				if (hasPlusSymbol == true)
				{
					counter += 4;
				}
				else if (hasPlusSymbol == false)
				{
					if (strcmp(opcode, "ADDR") == 0 || strcmp(opcode, "CLEAR") == 0 || strcmp(opcode, "COMPR") == 0 || strcmp(opcode, "DIVR") == 0 || strcmp(opcode, "MULR") == 0 || strcmp(opcode, "RMO") == 0 || strcmp(opcode, "SHIFTL") == 0 || strcmp(opcode, "SHIFTR") == 0 || strcmp(opcode, "SUBR") == 0 || strcmp(opcode, "SVC") == 0 || strcmp(opcode, "TIXR") == 0)
					{
						counter += 2;
					}
					else if (strcmp(opcode, "FIX") == 0 || strcmp(opcode, "FLOAT") == 0 || strcmp(opcode, "HIO") == 0 || strcmp(opcode, "NORM") == 0 || strcmp(opcode, "SIO") == 0 || strcmp(opcode, "TIO") == 0)
					{
						counter += 1;
					}
					else
					{
						counter += 3;
					}
				}
			}
			else if (strcmp(opcode, "BYTE") == 0) // this checks if the opcode is byte. then it check if the address starts with C or X
			{
				if (oparand[0] == 67) // the 67 is ASCII number for C
				{
					int c = 2;
					while (oparand[c++] != '\'')
						; // this counts all the bytes in C between two quotes
					c -= 3;
					if (c > 20)
					{
						// printf("Long Byte Constant is %s\n",oparand);
					}
					counter += c;
				}
				else if (oparand[0] == 88) // the 88 is ASCII for X
				{
					int x = 2;
					char hex[10]; // this sotes everything from X between two quotes
					while (oparand[x] != '\'')
					{
						hex[x - 2] = oparand[x];
						x++;
					}
					hex[x - 2] = '\0';
					// printf("%d hex return\n",check_for_hex(hex));
					if (check_for_hex(hex) == 0) // this check for invild hex digit.
					{
						printf("Testing case %s SIC Program Has Invalid Hex Constant\n", argv[1]);
						printf("The Invilid hex on line %d is %s", lines, fullline);
						fclose(fp);
						free(newsym);
						free(opcode);
						free(oparand);
						free(token_2);
						free(X_bit);
						free(base);
						free(fileName);
						for (int ab = 0; Symbol_table[ab]; ab++)
						{
							free(Symbol_table[ab]);
						}
						for (int ab = 0; LocTab[ab]; ab++)
						{
							free(LocTab[ab]);
						}
						for (int ab = 0; Hrecord[ab]; ab++)
						{
							free(Hrecord[ab]);
						}
						for (int ab = 0; Trecord[ab]; ab++)
						{
							free(Trecord[ab]);
						}
						for (int ab = 0; Mrecord[ab]; ab++)
						{
							free(Mrecord[ab]);
						}
						for (int ab = 0; Endrecord[ab]; ab++)
						{
							free(Endrecord[ab]);
						}
						fclose(OutputFile);
						return 0;
					}
					else
					{
						int X_bytelength = strlen(hex);
						// printf("line 196 %d\n",X);
						counter += (X_bytelength / 2);
					}
				}
			}
			else if (strcmp(opcode, "WORD") == 0) // this check if the opcode is word or not.
			{
				counter += 3;
			}
			// this check if the opcode is resb. if it is then the memory address is converted from string to integer and added to counter
			else if (strcmp(opcode, "RESB") == 0)

			{
				counter += atoi(oparand);
			}
			// this check if the opcode is resw. if it is then the memory address is converted from string to integer and multipled by 3 then added to counter
			else if (strcmp(opcode, "RESW") == 0)

			{
				counter = counter + (3 * atoi(oparand));
			}
		}
	}
	file_output(OutputFile, Hrecord);
	file_output(OutputFile, Trecord);
	file_output(OutputFile, Mrecord);
	file_output(OutputFile, Endrecord);

	free(newsym);
	free(opcode);
	free(oparand);
	free(token_2);
	free(X_bit);
	free(base);
	free(fileName);
	for (int ab = 0; Symbol_table[ab]; ab++)
	{
		free(Symbol_table[ab]);
	}
	for (int ab = 0; LocTab[ab]; ab++)
	{
		free(LocTab[ab]);
	}
	for (int ab = 0; Hrecord[ab]; ab++)
	{
		free(Hrecord[ab]);
	}
	for (int ab = 0; Trecord[ab]; ab++)
	{
		free(Trecord[ab]);
	}
	for (int ab = 0; Mrecord[ab]; ab++)
	{
		free(Mrecord[ab]);
	}
	for (int ab = 0; Endrecord[ab]; ab++)
	{
		free(Endrecord[ab]);
	}
	fclose(fp);
	fclose(OutputFile);
	return 0;
}

/************************************************************************************************
 *Down here all the function call																*
 *************************************************************************************************/
void formatOne(char *opcode, char *T_record[], int loction_counter)
{
	char *opcode_value = findOpcodeValue(opcode);
	char Text[70];
	char Op_address[7];
	memset(Text, '\0', 70 * sizeof(char));
	memset(Op_address, '\0', 7 * sizeof(char));
	Text[0] = 'T';
	sprintf(Op_address, "%06X", loction_counter);

	for (int k = 0; k < 6; k++)
	{
		Text[k + 1] = Op_address[k];
	}
	Text[7] = '0';
	Text[8] = '1';
	for (int k = 0; k < strlen(opcode_value); k++)
	{
		Text[k + 9] = opcode_value[k];
	}
	int null_termeneter = 0;
	null_termeneter = 8 + strlen(opcode_value);
	Text[null_termeneter + 1] = '\0';
	Addrecords(T_record, Text);
}

void formatTwo(char *opcode, char *opa, char *third_toekn, char *T_record[], int loction_counter)
{
	// printf("this is for the secound format test |%s|\t\t|%s|\t\t|%s|\n",ope,opa,third_toekn);
	char *opcode_value = findOpcodeValue(opcode);
	char *r1;
	char *r2;
	int r1_v = 0;
	int r2_v = 0;
	int op_v = 0;
	char op_h[8];
	char r1_h[4];
	char r2_h[4];
	char final_record[16];

	memset(op_h, '\0', 8 * sizeof(char));
	memset(r1_h, '\0', 4 * sizeof(char));
	memset(r2_h, '\0', 4 * sizeof(char));
	memset(final_record, '\0', 16 * sizeof(char));

	op_v = strtol(opcode_value, NULL, 16);
	r1 = findRegisterValues(opa);
	r2 = findRegisterValues(third_toekn);

	r1_v = strtol(r1, NULL, 16);
	if (r2 == NULL)
	{
		r2_v = 0;
	}
	else
	{
		r2_v = strtol(r2, NULL, 16);
	}
	sprintf(op_h, "%02X", op_v);
	sprintf(r1_h, "%01X", r1_v);
	sprintf(r2_h, "%01X", r2_v);
	for (int sd = 0; sd < 2; sd++)
	{
		final_record[sd] = op_h[sd];
	}
	for (int sd = 0; sd < 1; sd++)
	{
		final_record[sd + 2] = r1_h[sd];
	}
	for (int sd = 0; sd < 1; sd++)
	{
		final_record[sd + 3] = r2_h[sd];
	}

	char Text[70];
	char Op_address[7];
	memset(Text, '\0', 70 * sizeof(char));
	memset(Op_address, '\0', 7 * sizeof(char));

	Text[0] = 'T';
	sprintf(Op_address, "%06X", loction_counter);

	for (int k = 0; k < 6; k++)
	{
		Text[k + 1] = Op_address[k];
	}

	Text[7] = '0';
	Text[8] = '2';
	for (int k = 0; k < strlen(final_record); k++)
	{
		Text[k + 9] = final_record[k];
	}
	Addrecords(T_record, Text);
	// printf("%s\t%s\n",opcode,final_record);
}

void formatThree(bool hasHashSymbol, bool hasAtSymbol, bool p_flag, bool b_flag, bool isValidInt, char *opcode, char *oparand, char *X_bit, char *T_record[], int f, int loction_counter)
{
	char *opcode_value = findOpcodeValue(opcode);
	// printf("this is for the test %s %s\n",opcode_value,opcode);
	int test = strtol(opcode_value, NULL, 16);
	int binaryInt = 0;
	char binaray[5];
	char Opcode_ni[9];
	char xbpe[5];
	char final_add[13];
	char final_record[25];
	char negative_test[21];
	memset(Opcode_ni, '\0', 9 * sizeof(char));
	memset(xbpe, '\0', 5 * sizeof(char));
	memset(final_add, '\0', 13 * sizeof(char));
	memset(final_record, '\0', 25 * sizeof(char));
	memset(binaray, '\0', 5 * sizeof(char));
	memset(negative_test, '\0', 21 * sizeof(char));
	if (isValidInt == false && p_flag == false && b_flag == false)
	{
		test = test + 0;
		if (strcmp(X_bit, "X") == 0)
		{
			binaray[0] = '1';
		}
		else
		{
			binaray[0] = '0';
		}
	}
	else
	{
		if (hasHashSymbol == true && hasAtSymbol == false)
		{
			test += 1;
		}
		else if (hasHashSymbol == false && hasAtSymbol == true)
		{
			test += 2;
		}
		else if (hasHashSymbol == false && hasAtSymbol == false)
		{
			test += 3;
		}
		if (strcmp(X_bit, "X") == 0)
		{
			binaray[0] = '1';
		}
		else
		{
			binaray[0] = '0';
		}
		if (p_flag == true)
		{
			binaray[1] = '0';
			binaray[2] = '1';
		}
		if (p_flag == false)
		{
			binaray[1] = '1';
			binaray[2] = '0';
		}
		if (p_flag == false && b_flag == false)
		{
			binaray[1] = '0';
			binaray[2] = '0';
		}
		binaray[3] = '0';
	}

	int update_length = 0;

	binaryInt = strtol(binaray, NULL, 2);
	// printf("this is the value of binaryInt %02X	%d	%s\n",binaryInt,binaryInt,binaray);
	sprintf(Opcode_ni, "%02X", test);
	sprintf(xbpe, "%01X", binaryInt);
	// printf("this is the address been passed %04X	%s\n",f,oparand);
	if (f < 0)
	{
		// printf("it is coming in if statment\n");
		sprintf(negative_test, "%05X", f);
		// printf("this is foar the negative test %s		%ld\n",negative_test,strlen(negative_test));
		for (int sd = 0; sd < 3; sd++)
		{
			final_add[sd] = negative_test[sd + 5];
		}
	}
	else
	{
		sprintf(final_add, "%03X", f);
	}
	// printf("length of each record %ld 	%ld	%ld		%d\n",strlen(Opcode_ni),strlen(xbpe),strlen(final_add), x);
	for (int sd = 0; sd < strlen(Opcode_ni); sd++)
	{
		final_record[sd] = Opcode_ni[sd];
	}

	for (int sd = 0; sd < strlen(xbpe); sd++)
	{
		final_record[sd + 2] = xbpe[sd];
	}
	for (int sd = 0; sd < strlen(final_add); sd++)
	{
		final_record[sd + 3] = final_add[sd];
	}
	update_length = strlen(Opcode_ni) + strlen(xbpe) + strlen(final_add);
	final_record[update_length] = '\0';
	char Text[70];
	char Op_address[7];
	memset(Text, '\0', 70 * sizeof(char));
	memset(Op_address, '\0', 7 * sizeof(char));

	Text[0] = 'T';
	sprintf(Op_address, "%06X", loction_counter);

	for (int k = 0; k < 6; k++)
	{
		Text[k + 1] = Op_address[k];
	}

	Text[7] = '0';
	Text[8] = '3';
	for (int k = 0; k < strlen(final_record); k++)
	{
		Text[k + 9] = final_record[k];
	}
	Addrecords(T_record, Text);
	// printf("%s\t		|%s|		%ld		%ld\n",Text,final_record,strlen(Text),strlen(final_record));
}

void formatFour(bool hasHashSymbol, bool hasAtSymbol, bool p_flag, bool b_flag, char *opcode, char *oparand, char *X_bit, char *T_record[], int f, int loction_counter)
{
	char *opcode_value = findOpcodeValue(opcode);
	int test = strtol(opcode_value, NULL, 16);
	int binaryInt = 0;
	char binaray[5];
	char Opcode_ni[9];
	char xbpe[5];
	char final_add[21];
	char final_record[25];
	char negative_test[21];
	memset(Opcode_ni, '\0', 9 * sizeof(char));
	memset(xbpe, '\0', 5 * sizeof(char));
	memset(final_add, '\0', 21 * sizeof(char));
	memset(final_record, '\0', 25 * sizeof(char));
	memset(binaray, '\0', 5 * sizeof(char));
	memset(negative_test, '\0', 21 * sizeof(char));
	if (hasHashSymbol == true && hasAtSymbol == false)
	{
		test += 1;
	}
	else if (hasHashSymbol == false && hasAtSymbol == true)
	{
		test += 2;
	}
	else if (hasHashSymbol == false && hasAtSymbol == false)
	{
		test += 3;
	}
	if (strcmp(X_bit, "X") == 0)
	{
		binaray[0] = '1';
	}
	else
	{
		binaray[0] = '0';
	}
	if (p_flag == true)
	{
		binaray[1] = '0';
		binaray[2] = '1';
	}
	if (p_flag == false)
	{
		binaray[1] = '1';
		binaray[2] = '0';
	}
	if (p_flag == false && b_flag == false)
	{
		binaray[1] = '0';
		binaray[2] = '0';
	}
	binaray[3] = '1';
	if (f < 0)
	{
		// printf("it is coming in if statment\n");
		sprintf(negative_test, "%05X", f);
		// printf("this is foar the negative test %s		%ld\n",negative_test,strlen(negative_test));
		for (int sd = 0; sd < 3; sd++)
		{
			final_add[sd] = negative_test[sd + 5];
		}
	}
	else
	{
		sprintf(final_add, "%03X", f);
	}
	int update_length = 0;

	binaryInt = strtol(binaray, NULL, 2);
	// printf("this is the value of binaryInt %02X	%d	%s\n",binaryInt,binaryInt,binaray);
	sprintf(Opcode_ni, "%02X", test);
	sprintf(xbpe, "%01X", binaryInt);
	sprintf(final_add, "%05X", f);
	// printf("length of each record %ld 	%ld	%ld		%d\n",strlen(Opcode_ni),strlen(xbpe),strlen(final_add), x);
	for (int sd = 0; sd < strlen(Opcode_ni); sd++)
	{
		final_record[sd] = Opcode_ni[sd];
	}

	for (int sd = 0; sd < strlen(xbpe); sd++)
	{
		final_record[sd + 2] = xbpe[sd];
	}
	for (int sd = 0; sd < strlen(final_add); sd++)
	{
		final_record[sd + 3] = final_add[sd];
	}
	update_length = strlen(Opcode_ni) + strlen(xbpe) + strlen(final_add);
	final_record[update_length] = '\0';
	// printf("%s\t%s\n",opcode,final_record);
	char Text[70];
	char Op_address[7];
	memset(Text, '\0', 70 * sizeof(char));
	memset(Op_address, '\0', 7 * sizeof(char));

	Text[0] = 'T';
	sprintf(Op_address, "%06X", loction_counter);

	for (int k = 0; k < 6; k++)
	{
		Text[k + 1] = Op_address[k];
	}

	Text[7] = '0';
	Text[8] = '4';
	for (int k = 0; k < strlen(final_record); k++)
	{
		Text[k + 9] = final_record[k];
	}
	Addrecords(T_record, Text);
}
// this checks is given token is oparand or not. if the given token is oparand then it returns 1 else 0
int checkOpcode(char *test)
{
	for (int i = 0; i < 59; i++)
	{
		if (strcmp(test, Opcode_Table[i].Name) == 0)
		{
			// printf("it is in the table %s\n",test);
			return 1;
		}
	}
	return 0;
}

// here it checks if the given symbol already exits in symbol table.
int Symbol_Exists(SYMBOL *Tab[], char *Sname)
{
	int index = 0;
	int result;
	result = 0;

	while (Tab[index] != NULL)
	{
		if (strcmp(Tab[index]->Name, Sname) == 0)
		{
			result = -1;
			break;
		}
		index++;
	}
	return result;
}
int Symbol_Address(SYMBOL *Tab[], char *Sname)
{
	int index = 0;
	int result;
	result = -1;
	while (Tab[index] != NULL)
	{ // printf("this is in the symbol address |%s|\n",Tab[index]->Name);
		if (strcmp(Tab[index]->Name, Sname) == 0)
		{
			result = Tab[index]->Address;
			// printf("thie is in symbol address %X       %s\n ",result,Sname);
			return result;
			break;
		}
		index++;
	}
	return result;
}

// here all the new symbols are added to symbol table with their address, sorce line and name.
void AddSymbols(SYMBOL *Tab[], int Addr, int Srcline, char *Sname)
{
	int index1 = 0;
	SYMBOL *temp;
	temp = malloc(sizeof(SYMBOL));
	temp->Address = Addr;
	temp->DefinedOnSourceLine = Srcline;
	strcpy(temp->Name, Sname);
	while (Tab[index1] != NULL)
	{
		index1++;
	}
	Tab[index1] = temp;
}
void Addrecords(char *records[], char *record_data)
{
	int index2 = 0;
	char *temp;
	int check_size = strlen(record_data);
	temp = malloc(check_size * sizeof(record_data));
	memset(temp, '\0', check_size * sizeof(record_data));
	strcpy(temp, record_data);
	while (records[index2] != NULL)
	{
		index2++;
	}
	records[index2] = temp;
}

// this just prints out symbol table
void print_table(SYMBOL *Tab[])
{
	int index = 0;
	while (Tab[index] != NULL)
	{
		printf("%s\t\t %X\n", Tab[index]->Name, Tab[index]->Address);
		index++;
	}
}

// this checks if the given array contains invild hex digit if does then it returns -1 else 0
int check_for_hex(char hex[])
{
	int result;
	for (int i = 0; i < strlen(hex); i++)
	{
		if ((hex[i] >= 48 && hex[i] <= 57) || (hex[i] >= 65 && hex[i] <= 70) || (hex[i] >= 97 && hex[i] <= 102))
		{
			result = 1;
		}
		else
		{
			result = 0;
		}
	}
	return result;
}
// I find the opcode and the ther values
char *findOpcodeValue(char *opcodess)
{
	for (int i = 0; i < 59; i++)
	{
		if (strcmp(Opcode_Table[i].Name, opcodess) == 0)
		{
			return Opcode_Table[i].opCode;
		}
	}
	return NULL;
}

char *findRegisterValues(char *opcodess)
{
	for (int i = 0; i < 7; i++)
	{
		if (strcmp(Rs_table[i].Name, opcodess) == 0)
		{
			return Rs_table[i].value_of_rs;
		}
	}
	return NULL;
}
// write everyting to the output file
void file_output(FILE *Output_in, char *Output_write[])
{
	int index_test_index = 0;
	while (Output_write[index_test_index] != NULL)
	{
		// printf("this in in the file output %s\n",Output_write[index_test_index]);
		fprintf(Output_in, "%s\n", Output_write[index_test_index]);
		index_test_index++;
	}
}