#include <string.h>
#include <stdlib.h>

class Hero {
    char* m_name;
    double m_energy;

public:
 Hero (const char* name)
 : m_energy(10.0)
 {
     m_name = (char*) malloc(strlen(name)+1);
     strcpy(m_name,name);
 }
 ~Hero () 
 {
     free(m_name);
 }
 const char* GetName ()
 {
     return m_name;
 }
 void SetEnergy (double energy)
 {
     m_energy = energy;
 }
 double GetEnergy ()
 {
     return m_energy;
 }
};
