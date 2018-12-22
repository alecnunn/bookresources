/* Copyright (C) Jonty Barnes and Jason Hutchens, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jonty Barnes and Jason Hutchens, 2001"
 */

// C source of a character-level 1st-order Markov model.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc,char** argv) {
   int C[258][258],context,thresh,c=256,i,j;
   FILE* file;
   for(i=0; i<258; ++i)
      for(j=0; j<258; ++j)
         C[i][j]=0;
   if(argc!=2) return 1;
   file=fopen(argv[1],"r");
   if(!file) return 2;
   while(!feof(file)) {
      context=c; c=fgetc(file);
      if(c==-1) c=256;
      ++C[context][c]; ++C[context][257];
   }
   fclose(file);
   srand(time(NULL));
   do {
      context=c; c=-1;
      thresh=rand()%C[context][257];
      do {
         thresh-=C[context][++c];
} while(thresh>=0);
      fprintf(stdout,"%c",(char)c);
   } while(c!=256);
   return 0;
}

