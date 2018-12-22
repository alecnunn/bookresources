#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>

int ShiftRegister = 0x0001;
int Sector[2352];
int Scrambled[2352];
int Rescrambled[2352];

UpdateShiftRegister()
{
  int i;
  for(i = 0; i < 8;i++)
  {
    int hibit = ((ShiftRegister & 1)^((ShiftRegister & 2)>>1)) << 15;
    ShiftRegister = (hibit | ShiftRegister) >> 1;
  }
}

void Scramble()
{
  int i;
  for (i=12;i<2352;i++)
  {
    Scrambled[i] = Scrambled[i] ^ (ShiftRegister&0xFF);
    UpdateShiftRegister();
  }
}

main()
{
	int a;
	memset(Scrambled, 0, 2352);
    Scramble();
    for(a=0; a < 2352; a++)
    printf("%x ",Scrambled[a]);


}