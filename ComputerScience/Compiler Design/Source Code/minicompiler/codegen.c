#include "cic.h"
#include<elf.h>
#define MAXSECTIONS 100
#define MAXPROGRAMS 100
#define MAXSYMBOLS 100

void dispMainCode()
{
	int i;
	for (i = 0;i < cc; i++)
	{
		printf("%x ",mainCode[i]);
	}
	printf("\n");
}
void genCodeFromQuad(Quad *quadListHeader)
{
	unsigned char ch=0;
	int spaceInStack=0;
	unsigned char byte1=0,byte2=0,byte3=0,byte4=0;
	Quad *temp;
	List L;
	int symCount=0;
	int arg1,arg2,quo,rem;
	unsigned char stackPos;
	st = gst;
 	temp=quadListHeader->nextQuad;
 	while(temp!=NULL)
	{
		//printf("%d: %s\n",temp->operator,ops[temp->operator]);
		switch(temp->operator)
        	{
			case ASSIGN: 
				if(temp->operand1->type==CONST)
				{
					arg1 = temp->operand1->value.iVal;
					byte1 = arg1 & 0x000000ff;
					byte2 = (arg1 >> 8) & 0x000000ff;
					byte3 = (arg1 >> 16) & 0x000000ff;
					byte2 = (arg1 >> 24) & 0x000000ff;
					stackPos = 0xff - getStackPos(st,temp->result->name)+1;	
					mainCode[cc++] = 0xc7;	
					mainCode[cc++] = 0x45;	
					mainCode[cc++] = stackPos;	
					mainCode[cc++] = byte1;	
					mainCode[cc++] = byte2;	
					mainCode[cc++] = byte3;	
					mainCode[cc++] = byte4;	
				}
				else
				{
					stackPos = 0xff - getStackPos(st,temp->operand1->name)+1;	
					//mov operand1stackPos (%ebp), %eax
				//	mainCode[cc++] = 0x8b;	
				//	mainCode[cc++] = 0x45;	
				//	mainCode[cc++] = stackPos;	
				}
				break;
			case FUNCTION: 
				L=findSymbolHash(st,temp->result->name);
				if(L==NULL)
				{
					printf("required symbol table is not found\n");
					exit(-1);
				}
				st = L->symbol.st;
				//To prepare the activation record ready on the stack
				symCount=countSymbols(st);		
				spaceInStack = (int)(ceil(((double)symCount + 4)/16)) * 16  ;
				assignLocInStack(st,spaceInStack);	
				//Store ebp on the stack and ebp holds the stack pointer to work across the Activation Record
				mainCode[cc++] = 0x55 ;     //push %ebp
        			mainCode[cc++] = 0x89;      //mov %esp,%ebp
        			mainCode[cc++] = 0xe5;     
				//Calculate the space requirements on the stack to hold all the local variables
				ch = spaceInStack & 0x000000ff; //This space may increase more than 256 bytes - Code to be modified then
				mainCode[cc++] = 0x83;	//sub $chvalue, %esp
				mainCode[cc++] = 0xec;
				mainCode[cc++] = ch;
				break;
			case RETURN: 
				//stackPos = 0xff - getStackPos(st,temp->operand1->name)+1;	
				//mov stackPos(%epb), eax
				mainCode[cc++] = 0x8b;
                                mainCode[cc++] = 0x45;
                                mainCode[cc++] = stackPos;
				//leave
				//ret	
                                mainCode[cc++] = 0xc9;
                                mainCode[cc++] = 0xc3;
				st=st->prevst;
				break;
                	case PLUS:
				if(temp->operand1->type==CONST)
				{
					arg1 = temp->operand1->value.iVal;
					byte1 = arg1 & 0x000000ff;
					byte2 = (arg1 >> 8) & 0x000000ff;
					byte3 = (arg1 >> 16) & 0x000000ff;
					byte2 = (arg1 >> 24) & 0x000000ff;
				}
				else
				{
					stackPos = 0xff - getStackPos(st,temp->operand1->name)+1;	
					//mov operand1stackPos (%ebp), %eax
					mainCode[cc++] = 0x8b;	
					mainCode[cc++] = 0x45;	
					mainCode[cc++] = stackPos;	
				}
				if(temp->operand2->type==CONST)
				{
					arg2 = temp->operand2->value.iVal;
					byte1 = arg2 & 0x000000ff;
					byte2 = (arg2 >> 8) & 0x000000ff;
					byte3 = (arg2 >> 16) & 0x000000ff;
					byte2 = (arg2 >> 24) & 0x000000ff;
				}
				else
				{
					stackPos = 0xff - getStackPos(st,temp->operand2->name)+1;	
					//add operand2stackPos (%ebp), %edx
					mainCode[cc++] = 0x8b;
                                        mainCode[cc++] = 0x55;
                                        mainCode[cc++] = stackPos;
				}
				//lea(%edx,%eax,1), %eax
				mainCode[cc++] = 0x8d;
                                mainCode[cc++] = 0x04;
                                mainCode[cc++] = 0x02;
				//mov %eax,stackPos(%ebp) 	
				stackPos = 0xff - getStackPos(st,temp->result->name)+1;	
				//mov  %eax, resultstackPos(%ebp)
				mainCode[cc++] = 0x89;
				mainCode[cc++] = 0x45;	
				mainCode[cc++] = stackPos;	
                        	break;
                	case MINUS:
				//code similar to ADD
                        	break;
                	case MUL:
				//Code similar to ADD
                        	break;
			case DIV:
				//Code similar to ADD
                        	break;
			case MODULO:
				//Code similar to ADD
                        	break;
			default: break;
        	}
		temp=temp->nextQuad;
	}
	mainCodeSize = cc;
printf("codesize = %d\n",mainCodeSize);
	dispMainCode();
}

int createElfFile(char *dstFile)
{
        int i,j,k,pos,offset,size,noSymbols;
	int pos1,pos2;
        int type,binding;
        int R,W,E;
        char *stringTable,*symStrTable;
        FILE *fpi,*fpo;
	char *buf;
	char **segmentsIn, **segmentsOut;
	char **sectionsIn, **sectionsOut;
        Elf32_Ehdr *ehdrIn,*ehdrOut;
        //Elf32_Shdr shdrIn,shdrsIn[MAXSECTIONS];
        Elf32_Shdr shdrOut,**shdrsOut;
        Elf32_Shdr shdrIn,**shdrsIn;
        //Elf32_Phdr phdrIn,phdrsIn[MAXPROGRAMS];
        Elf32_Phdr phdrOut,**phdrsOut;
        Elf32_Phdr phdrIn,**phdrsIn;
        //Elf32_Phdr phdrsOut[MAXPROGRAMS];
        Elf32_Sym *sym_tab_ptr,*symTable, symbols[MAXSYMBOLS];
        char *shdr_ptr;
        int result;

	//Opening the ELF file for both reading and writing
	fpi = fopen("elftemplate","rb");
	if(NULL == fpi)
	{
		printf("Error in reading elf template file .. Program terminated\n");
		exit(-1);
	}
        fpo=fopen(dstFile,"wb");
        if(NULL==fpo)
        {
                printf("Error in creating the elf file .. Program terminated \n",dstFile);
                exit(-1);
        }
	ehdrIn = (Elf32_Ehdr*) malloc(sizeof(Elf32_Ehdr));
	ehdrOut = (Elf32_Ehdr*) malloc(sizeof(Elf32_Ehdr));
	fseek(fpi,0,SEEK_END);
	size = ftell(fpi);
	buf = (char*) malloc(size);
	fwrite( (char*)buf, size,1,fpo);
	/*Reading the Elf32_Ehdr for making copy of the Elf32_EHdr for writing into another Elf32_Ehdr*/
	fseek(fpi,0,SEEK_SET);	
	result = fread((char*)ehdrIn,sizeof(Elf32_Ehdr),1,fpi);
        if(result <= 0)
        {
                printf("ElfHeader could not be read from the elf template file\n");
                return -1;
        }

	//Copy the Elf32_Ehdr from an existing Elf file format to the new file to be created
	memcpy((char*)ehdrOut,(char*)ehdrIn,sizeof(Elf32_Ehdr));
	fseek(fpo,0,SEEK_SET);	
	result = fwrite((char*)ehdrOut,sizeof(Elf32_Ehdr),1,fpo);
        if(result <= 0)
        {
                printf("ElfHeader could not be written to %s\n",dstFile); 
                return -1;
        }

	if(ehdrIn->e_phnum > 0)
	{
		//Memory Allocation for program headers
		phdrsIn = (Elf32_Phdr**) malloc(sizeof(long int)*ehdrIn->e_phnum);	
		for(i=0;i<ehdrIn->e_phnum;i++)
			phdrsIn[i] = (Elf32_Phdr*)malloc(sizeof(Elf32_Phdr));
		phdrsOut = (Elf32_Phdr**) malloc(sizeof(long int)*ehdrOut->e_phnum);	
		for(i=0;i<ehdrOut->e_phnum;i++)
			phdrsOut[i] = (Elf32_Phdr*)malloc(sizeof(Elf32_Phdr));

		//Reading the program headers and writing the program headers
		fseek(fpi,ehdrIn->e_phoff,SEEK_SET);
		fseek(fpo,ehdrOut->e_phoff,SEEK_SET);
        	for(i=0;i<ehdrIn->e_phnum;i++)
        	{
                	result = fread((char*)phdrsIn[i],sizeof(Elf32_Phdr) ,1,fpi);
                	if(result <= 0)
                	{
                        	printf("Error in reading the Program header from Elf template file: %s\n"); 
                        	exit(-1);
                	}
			memcpy((char*)phdrsOut[i],(char*)phdrsIn[i],sizeof(Elf32_Phdr));
                	result = fwrite((char*)phdrsOut[i],sizeof(Elf32_Phdr) ,1,fpo);
                	if(result <= 0)
                	{
                        	printf("Error in writing the section  header into the file: %s\n",dstFile);
                        	exit(-1);
                	}
        	}
	}
	
	//Allocate memory for the section headers for the existing elf file and output elf file
	shdrsIn = (Elf32_Shdr**) malloc(sizeof(long int)*ehdrIn->e_shnum);	
	for(i=0;i<ehdrIn->e_shnum;i++)
		shdrsIn[i] = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr));

	shdrsOut = (Elf32_Shdr**) malloc(sizeof(long int)*ehdrOut->e_shnum);	
	for(i=0;i<ehdrOut->e_shnum;i++)
		shdrsOut[i] = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr));

	
	//Copy section headers from the exisiting elf file to the elfout file
	fseek(fpi,ehdrIn->e_shoff,SEEK_SET);
	fseek(fpo,ehdrOut->e_shoff,SEEK_SET);
        for(i=0;i<ehdrIn->e_shnum;i++)
        {
	
                result = fread((char*)shdrsIn[i],sizeof(Elf32_Shdr) ,1,fpi);
                if(result <= 0)
                {
                        printf("Error in reading the section header from the Elf template file\n");
                        exit(-1);
                }
		memcpy((char*)shdrsOut[i],(char*)shdrsIn[i],sizeof(Elf32_Shdr));
                result = fwrite((char*)shdrsOut[i],sizeof(Elf32_Shdr),1,fpo);
                if(result <= 0)
                {
                        printf("Error in Writing the section header to the file\n",dstFile); 
                        exit(-1);

                }
        }
	
	//Read the string table for all the display of names
	pos = shdrsIn[ehdrIn->e_shstrndx]->sh_offset;
	size = shdrsIn[ehdrIn->e_shstrndx]->sh_size;
	fseek(fpi,pos,SEEK_SET);
        stringTable = (char*)malloc(size); 
        result = fread((char*)stringTable,size,1,fpi);
        if(result <= 0)
        {
                printf("Could not read string table from the Elf Template file %s\n");
                return -1;
        }
	fseek(fpo,pos,SEEK_SET);
        result = fwrite((char*)stringTable,size,1,fpo);
        if(result <= 0)
        {
                printf("Could not write string table to  the out Elf file %s\n");
                return -1;
        }
	//Allocate Memory for holding the sections
	sectionsIn = (char**) malloc(ehdrIn->e_shnum*sizeof(long int));
	sectionsOut = (char**) malloc(ehdrOut->e_shnum*sizeof(long int));
        for(i=0;i<ehdrIn->e_shnum;i++)
        {
		sectionsIn[i] = (char*) malloc(shdrsIn[i]->sh_size);
		sectionsOut[i] = (char*) malloc(shdrsOut[i]->sh_size);
	}
	for(i=0;i<ehdrIn->e_shnum;i++)
        {
                if(shdrsIn[i]->sh_type==SHT_STRTAB)
                {
                        size = shdrsIn[i]->sh_size;
                        pos = shdrsIn[i]->sh_addr+shdrsIn[i]->sh_offset;
        		fseek(fpi,pos,SEEK_SET);
        		fseek(fpo,pos,SEEK_SET);
                        symStrTable = (char*)malloc(size);
                        fread((char*)symStrTable,size,1,fpi);
                        fwrite((char*)symStrTable,size,1,fpo);
		}
	}
                

	//Copying the sections to the elf out file
        for(i=0;i<ehdrIn->e_shnum;i++)
        {
		if(shdrsIn[i]->sh_size > 0)
		{
			fseek(fpi,shdrsIn[i]->sh_offset,SEEK_SET);	
                	result = fread((char*)sectionsIn[i],shdrsIn[i]->sh_size,1,fpi);
                	if(result <= 0)
                	{
                        	printf("Error in reading the section from the elf template elf file: \n"); 
                        	exit(-1);
                	}
			//Copy every sections from the existing elf file except .text portion which is to be produced 
			//from the compilation of the new program file
			if(strcmp((stringTable+shdrsIn[i]->sh_name),".text") == 0)
			{
				shdrsOut[i]->sh_size = mainCodeSize;
				memcpy((char*)sectionsOut[i],(char*)&mainCode[0], shdrsOut[i]->sh_size); 
			}
			else
			{
				memcpy((char*)sectionsOut[i],(char*)sectionsIn[i],shdrsIn[i]->sh_size); 
			}
		
			fseek(fpo,shdrsOut[i]->sh_offset,SEEK_SET);	
                	result = fwrite((char*)sectionsOut[i],shdrsOut[i]->sh_size,1,fpo);
                	if(result <= 0)
                	{
                        	printf("Error in writing the section to the file: %s\n", dstFile); 
                        	exit(-1);
                	}
			
		}
	}

	//copy remaining portion of the files into the target elf file
	pos1=ftell(fpi);
        fseek(fpi,0,SEEK_END);
        pos2=ftell(fpi);
        size = pos2-pos1;
        buf=(char*)malloc(sizeof(char)*size);
        fread((char*)buf,size,1,fpi);
        fwrite((char*)buf,size,1,fpo);
	fclose(fpi);
	fclose(fpo);
}
