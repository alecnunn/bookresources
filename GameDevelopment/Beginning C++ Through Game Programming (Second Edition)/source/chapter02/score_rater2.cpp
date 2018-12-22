// Score Rater 2.0
// Demonstrates an else clause

#include <iostream>
using namespace std;

int main() 
{
	int score;
	cout << "Enter your score: ";
	cin >> score;

	if (score > 500)
		cout << "\nYou got over 500. Nice score.\n";
	else
		cout << "\nYou got 500 or less. Nothing to brag about.\n";

    return 0;
}

