#include<stdio.h>
#include<elf.h>
#include<stdlib.h>
#include<string.h>
#define MAXSECTIONS 100
#define MAXPROGRAMS 100
#define MAXSYMBOLS 100
#define teste {printf("Test and exit ..\n");exit(-1);}
#define testc {printf("Test and continue ..\n");}

int main(int ac,char *av[])
{
        int i,j,k,pos,size,noSymbols;
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

	//Check for the command line Usage:
	if(ac  != 3)
	{
		printf("Improperly formed command\n");
		printf("Usage: mcp srcfile dstfile\n");
		exit(-1);
	}

	//Opening the ELF file for both reading and writing
	fpi = fopen(av[1],"rb");
	if(NULL == fpi)
	{
		printf("Error in reading elf file .. Program terminated\n",av[1]);
		exit(-1);
	}
        fpo=fopen(av[2],"wb");
        if(NULL==fpo)
        {
                printf("Error in creating the elf file .. Program terminated \n",av[2]);
                exit(-1);
        }
	ehdrIn = (Elf32_Ehdr*) malloc(sizeof(Elf32_Ehdr));
	ehdrOut = (Elf32_Ehdr*) malloc(sizeof(Elf32_Ehdr));

	/*Reading the Elf32_Ehdr for making copy of the Elf32_EHdr for writing into another Elf32_Ehdr*/
	fseek(fpi,0,SEEK_SET);	
	result = fread((char*)ehdrIn,sizeof(Elf32_Ehdr),1,fpi);
        if(result <= 0)
        {
                printf("ElfHeader could not be read from\n",av[1]);
                return -1;
        }

	//Copy the Elf32_Ehdr from an existing Elf file format to the new file to be created
	memcpy((char*)ehdrOut,(char*)ehdrIn,sizeof(Elf32_Ehdr));
	fseek(fpo,0,SEEK_SET);	
	result = fwrite((char*)ehdrOut,sizeof(Elf32_Ehdr),1,fpo);
        if(result <= 0)
        {
                printf("ElfHeader could not be written to %s\n",av[2]);
                return -1;
        }

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
                        printf("Error in reading the Program header from the file: %s\n",av[1]); 
                        exit(-1);
                }
		memcpy((char*)phdrsOut[i],(char*)phdrsIn[i],sizeof(Elf32_Phdr));
                result = fwrite((char*)phdrsOut[i],sizeof(Elf32_Phdr) ,1,fpo);
                if(result <= 0)
                {
                        printf("Error in writing the section  header into the file: %s\n",av[2]); 
                        exit(-1);
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
                        printf("Error in reading the section header from the file\n",av[1]); 
                        exit(-1);
                }
		memcpy((char*)shdrsOut[i],(char*)shdrsIn[i],sizeof(Elf32_Shdr));
                result = fwrite((char*)shdrsOut[i],sizeof(Elf32_Shdr),1,fpo);
                if(result <= 0)
                {
                        printf("Error in Writing the section header to the file\n",av[2]); 
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
                printf("Could not read string table from the file %s\n",av[1]);
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
                //if(shdrsIn[i]->sh_type==SHT_SYMTAB)
                if(shdrsIn[i]->sh_type==SHT_STRTAB)
                {
                        size = shdrsIn[i]->sh_size;
                        pos = shdrsIn[i]->sh_addr+shdrsIn[i]->sh_offset;
        		fseek(fpi,pos,SEEK_SET);
                        symStrTable = (char*)malloc(size);
			printf("size of the symbol table at section %d is: %d\n",i,size);
                        fread((char*)symStrTable,size,1,fpi);
			printf("\n\nSymbol String Table\n\n");
			for(k=0;k<size;k++)
				printf("%c ",symStrTable[k]);
			printf("\n");
		}
	}
                
	//Display Symbol table
	for(i=0;i<ehdrIn->e_shnum;i++)
	{
		if(shdrsIn[i]->sh_type == SHT_SYMTAB)
		{
			printf("symbol table found at section %d:\n",i);
                        pos = shdrsIn[i]->sh_offset;
                        size = shdrsIn[i]->sh_size;
                        fseek(fpi,pos,SEEK_SET);
                        noSymbols = size/sizeof(Elf32_Sym);
			printf("noSymbols = %d\n",noSymbols);
                        printf("\t\t\tSymbol Table\n");
                        printf("Number  Value  Size    Type    Binding    Vis   Index   Name\n");
                        for(j = 0; j < noSymbols; j++)
                        {
                                result = fread((char*)&symbols[j],sizeof(Elf32_Sym) ,1,fpi);
                                if(result <= 0)
                                {
                                        printf("Error in reading the enough number of bytes for the symbol table\n");
                                        exit(-1);
                                }
                                printf("%d\t",j);
                                printf("%x  ",symbols[j].st_value);
                                printf("%d   ",symbols[j].st_size);
                                binding = symbols[j].st_info >> 4;
                                type = symbols[j].st_info & 0xf;

                                switch(type)
                                {
                                        case STT_NOTYPE: printf("NOTYPE\t");break;
                                        case STT_OBJECT: printf("OBJECT\t");break;
                                        case STT_FUNC:   printf("FUNCTION\t");break;
                                        case STT_SECTION: printf("SECTION\t");break;
                                        case STT_FILE:   printf("FILE\t");break;
					case STT_LOPROC: printf("LOPROC\t");break;
                                        case STT_HIPROC: printf("HIPROC\t");break;
                                        default: printf("Unknown\t");
                                }
                                switch(binding)
                                {
                                        case STB_LOCAL: printf("LOCAL\t"); break;
                                        case STB_GLOBAL:  printf("GLOBAL\t"); break;
                                        case STB_WEAK:  printf("WEAK\t");break;
                                        default: printf("Unknown\t");
                                }
                                switch(symbols[j].st_other)
                                {
                                        case  STV_DEFAULT: printf("DEFAULT\t");break;
                                        case  STV_INTERNAL: printf("INTERNAL\t");break;
                                        case  STV_HIDDEN: printf("HIDDEN\t");break;
                                        case  STV_PROTECTED: printf("NOTPREEMPTABLE\t");break;
                                        default: printf("Unkown\t");
                                }
                                switch(symbols[j].st_shndx)
                                {
                                        case SHN_UNDEF: printf("UND\t");break;
                                        case SHN_HIPROC: printf("HIPROC\t");break;
                                        case SHN_ABS:   printf("ABS\t");break;
                                        case SHN_COMMON: printf("COMMON\t");break;
                                        case SHN_HIRESERVE: printf("HIRES\t");break;
                                        default: printf("%d\t",symbols[j].st_shndx);
                                }
                                if(symbols[j].st_name > 0)
                                        printf("[%d]: %s\n",j,(symStrTable+symbols[j].st_name));
                                else
                                        printf("\n");
			}
			printf("\n");
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
                        	printf("Error in reading the section from the file: \n",av[1]); 
                        	exit(-1);
                	}
			//Copy every sections from the existing elf file except .text portion which is to be produced 
			//from the compilation of the new program file
			//Change this portion of this text section to import the machine code generated from the given source file
			/*
			if(strcmp((stringTable+shdrsIn[i]->sh_name),".text") == 0)
			{
				genTextCode();
				memcpy((char*)sectionsOut[i],(char*)textCode, textCodeSize); 
			}
			else
			*/
				memcpy((char*)sectionsOut[i],(char*)sectionsIn[i],shdrsIn[i]->sh_size); 
			fseek(fpo,shdrsOut[i]->sh_offset,SEEK_SET);	
                	result = fwrite((char*)sectionsOut[i],shdrsOut[i]->sh_size,1,fpo);
                	if(result <= 0)
                	{
                        	printf("Error in writing the section to the file: %s\n",av[2]); 
                        	exit(-1);
                	}
			
		}
	}

 
	//printf("Initial position of the output file = %x  and position of the input file=%x  \n",ftell(fpo),ftell(fpi));
	printf("shstrndx=%d shentsize = %d\n",ehdrIn->e_shstrndx,ehdrIn->e_shentsize);

	
	printf("\n-------------------------------------------------------------------------------------------\n");
        printf("\n                              Printing the Section Headers\n");
        printf("\n-------------------------------------------------------------------------------------------\n");
        printf("Sl.No\tName\tType\tFlags\tAddress\tOffset\tSize\tLink\tInfo\tAAlign\tEntSize\n");

	for(i=0;i<ehdrIn->e_shnum;i++)
        {
		printf("%d: ",i);
		printf("%s\t",(stringTable+shdrsIn[i]->sh_name));
		printf("%x\t",shdrsIn[i]->sh_type);
		printf("%x\t",shdrsIn[i]->sh_flags);
		printf("%x\t",shdrsIn[i]->sh_addr);
		printf("%x\t",shdrsIn[i]->sh_offset);
		printf("%x\t",shdrsIn[i]->sh_size);
		printf("%x\t",shdrsIn[i]->sh_link);
		printf("%x\t",shdrsIn[i]->sh_info);
		printf("%x\t",shdrsIn[i]->sh_addralign);
		printf("%x\n",shdrsIn[i]->sh_entsize);
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
