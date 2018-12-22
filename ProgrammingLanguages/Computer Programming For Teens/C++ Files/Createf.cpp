// Chapter 16 : Creates an external file

#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>

int main ()
{

int x;
char phone[8]; int m;
ofstream fout;
const char * file = "Phone_Numbers.dat";
 fout.open( file, ios::out |ios::binary);

 if (!fout.is_open())
{
  cerr<<"can't open "<< file <<" file for writing.\n";
  exit(1);
}

else

 for (x = 0; x < 10; x++)
 {

 cout << "Please type a phone number to send to a file."<< endl;
 for (m=0;m<8;m++)
{
cin >> phone[m];
}
fout <<phone<< "\n"; // sends the phone number out to the file



 }

 fout.close();
 
return 0;
}


