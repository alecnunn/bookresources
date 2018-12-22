
// Chapter 22 (online): the implementation file
Person::Person (char n1[10],char a1[10], char p1[8], int num )         // a constructor
{ for (int m= 0; m < 10; m++)
{
 name[m] = n1[m];
address[m] = a1[m];
}
for (int g= 0; g < 8; g++)
{phone[g] = p1[g];}
age = num;
}
void Person:: Print_Info()
{
cout << "Name : ";
 for (int v= 0; v < 10; v++)
{
 cout<<name[v];
}
 cout<< endl;


cout << "Address : ";
 for (v= 0; v < 10; v++)
{
 cout<<address[v];
}
 cout<< endl;


cout << "Phone Number : ";
 for ( v= 0; v < 8; v++)
{
 cout<<phone[v];
}
 cout<< endl;


cout << "Age : " << age << endl;
cout<<endl;
}


void Person::Change_Address( char address2[10])
{
//wiping out the old address
for ( int d = 0; d < 10; d++)
address[d]=' ';
 
//replacing one address with the parameter given
for ( d = 0; d < 10; d++)
address[d] = address2[d];
}

void Person::Change_Age( int year)
{
age = year;
}





