#include "headers.h"



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
