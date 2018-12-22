/****************************************************************
 * CLASS Chromosome
 * The genetic material for our troll.  Very simple indeed.
 ***************************************************************/

#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include "Globals.h"

struct Chromosome
{
	double Priorities[ ALL_PRIORITIES ];
};

#endif