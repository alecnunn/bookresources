#include <string.h>
#include <stdlib.h>

#include "actor.h"

class Hero : public Actor
{
    double m_energy;

public:
 Hero (const char* name)
 : Actor(name), m_energy(10.0)
 {
 }
 ~Hero () 
 {
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
