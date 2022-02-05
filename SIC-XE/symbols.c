#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>



int IsAValidSymbol( char *TestSymbol ){

	int Result = 1;
	int index =0;
	int maxlength =6;
	int done;
	done =0;

	while(!done)
	{
		if(TestSymbol[index]=='\0')
		{
			break;
		}
		if(index == maxlength)
		{
			done =1;
			Result =0;
		}
		if((TestSymbol[index]==36) || (TestSymbol[index]==33) || (TestSymbol[index]==61) ||
		   (TestSymbol[index]==43) || (TestSymbol[index]==45) ||(TestSymbol[index]==40) ||
		   (TestSymbol[index]==41) || (TestSymbol[index]==64))
		{
			done =1;
			Result =0;
		}
		
		index++;
	}

	return Result;
}
void Addloc(struct locStored *Tab[], int current, int Fline){
	int index;
	index = 0;

	struct locStored* newloc;
	newloc = malloc(sizeof(struct locStored));
	newloc->locCtr = current; 
	newloc->Fline = Fline; 

	while(Tab[index] != NULL){
    index++;
    }
    Tab[index] = newloc;
	//printf("|%d | |%X  | \n", Fline, current); 
}
int getLocCtr(struct locStored *Tab[], int Fline){
	int returnedVal = 0;
	int index =0;
	while(Tab[index] != NULL){
		//printf("%s \n", Tab[index]->Name);

		if(Tab[index]->locCtr == Fline)
		{
			//printf("%X\n",Tab[index]->locCtr);
		if(Tab[index +1 ] == NULL)
		{
			break;
		}
		else
		{
			
			returnedVal = Tab[index +1]->locCtr; 
		}
		break;
		}
	index++;
	}
  return returnedVal;
}
