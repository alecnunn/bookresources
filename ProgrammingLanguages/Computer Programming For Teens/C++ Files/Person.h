
//Chapter 22 (online): the header file

#ifndef _PERSON_H
#define _PERSON_H

class Person
{
public:
Person (char n1[10],char a1[10], char p1[8], int num );
void Print_Info (   ) ;
void Change_Address ( char address2[10]);
void Change_Age ( int year);

private:

char  name[10]; char address[10]; char phone[8];
int age;
};


#endif
