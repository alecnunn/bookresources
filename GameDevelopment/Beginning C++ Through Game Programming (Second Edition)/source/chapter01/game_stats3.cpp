// Game Stats 3.0
// Demonstrates constants

#include <iostream>
using namespace std;

int main()
{
	const int ALIEN_POINTS = 150;
	int aliensKilled = 10;
	int score = aliensKilled * ALIEN_POINTS;
	cout << "score: " << score << endl;

	enum difficulty {NOVICE, EASY, NORMAL, HARD, UNBEATABLE};
	difficulty myDifficulty = EASY;

	enum ship {FIGHTER = 25, BOMBER, CRUISER = 50, DESTROYER = 100};
	ship myShip = BOMBER;
	cout << "\nTo upgrade my ship to a Cruiser will cost " 
         << (CRUISER - myShip) << " Resource Points.\n";

	return 0;
}
