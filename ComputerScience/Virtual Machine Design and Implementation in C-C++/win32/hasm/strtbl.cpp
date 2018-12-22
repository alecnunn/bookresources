/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ strtbl.cpp - the string table ( init, add elements, print )       +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ Macros                                                            +                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define STR_TBL_INIT  2*1024	/*initial size of string table*/
#define STR_TBL_INC	  1024		/*amount increment when expand*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declaration                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class StringTable
{
	U8 nStr;	/*current capacity*/
	
	public:
	U1 *text;
	U8 iStr;	/*next free space*/

	StringTable();
	~StringTable();
	void addStrTbl(char *str);
	void printStrTbl();
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

StringTable::StringTable()
{
	text = (U1*)malloc(STR_TBL_INIT);
	if(text==NULL)
	{
		ERROR0("StringTable::StringTable(): out of memory\n");
		FATAL_ERROR();
	}
	iStr = 0;
	nStr = STR_TBL_INIT;
	return;

}/*end constructor*/

/*-----------------------------------------------------------------*/

StringTable::~StringTable()
{
	free(text);
	return;

}/*end destructor*/

/*-----------------------------------------------------------------*/

/*
add string to strTbl ( re-size if necessary )
*/

void StringTable::addStrTbl(char *str)
{
	U8 len;
	U8 i;
	U8 final_index;
	U1 *ptr;

	len = strlen(str);	/*note: null not included*/
	len++;				/*count null*/
	final_index = iStr + len - 1;

	/* capacity n will have indices 0,...,n-1 */

	if(final_index >= nStr)
	{
		ptr = (U1*)malloc((size_t)(nStr+STR_TBL_INC));
		if(ptr==NULL)
		{
			ERROR0("StringTable::addStrTbl(): no more memory\n");
			FATAL_ERROR();
		}
		else
		{
			for(i=0;i<nStr;i++){ ptr[i] = text[i]; }
			free(text);
			text = ptr;
			nStr = nStr+STR_TBL_INC;
		}
	}
	
	for(i=0;i<=len;i++)
	{ 
		text[iStr+i] = str[i];
	}
	iStr = final_index+1; /* next EMPTY space */
	return;

}/*end addStrTbl*/

/*-----------------------------------------------------------------*/

void StringTable::printStrTbl()
{
	U8 i;
	U8 nstr;

	printf("\nSTRING TABLE-----------------------\n");
	printf("capacity nStr ="); pU8(nStr); printf("\n");
	printf("next free index iStr ="); pU8(iStr); printf("\n\n");
	nstr = 1;
	
	for(i=0;i<iStr;i++)
	{
		if(i==0){printf("0)");}

		/*either print char or handle null char*/

		if(text[i]!='\0')
		{ 
			putchar(text[i]); 
		}
		else if(i<(iStr-1))
		{ 
			printf("(null)\n%lu)",nstr); nstr++; 
		}
		else if(i==(iStr-1))
		{ 
			printf("(null)\n");
		}
	}
	return;

}/*printStrTbl*/