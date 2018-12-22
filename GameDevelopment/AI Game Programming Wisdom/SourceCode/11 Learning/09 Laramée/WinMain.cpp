#include "Globals.h"
#include "Entities.h"
#include "Genetic.h"
#include "Troll.h"
#include <stdlib.h>
#include <iostream.h>
#include <time.h>
#include "Simulation.h"
#include <Windows.h>

int main( int argc, char ** argv )
{
	srand( time( 0 ) );

	DWORD timer = GetTickCount();

	cout << "One moment please..." << endl;

	Genetic ga;
	ga.RunEvolution();

	cout << "Duration of the experiment: " << (double) ( GetTickCount() - timer ) / 1000.0
	     << " seconds." << endl;

	return 0;
}