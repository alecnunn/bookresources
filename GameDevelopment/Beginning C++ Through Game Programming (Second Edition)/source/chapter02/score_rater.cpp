// Score Rater
// Demonstrates the if statement

#include <iostream>
using namespace std;

int main() 
{
	if (true)
		cout << "This is always displayed.\n\n";

	if (false)
		cout << "This is never displayed.\n\n";

	int score = 1000;

	if (score)
		cout << "Okay, at least you didn't score zero.\n\n";
	
	if (score > 500)
		cout << "You scored over 500.  Nice.\n\n";

	if (score == 1000)
	{
		cout << "You scored a perfect 1000!\n";
		cout << "Now that's impressive.\n\n";
	}

    if (score > 500)
    {
        cout << "You scored at least 500.\n";
        if (score >= 1000)
            cout << "You scored 1000 or more!\n";
    }

    return 0;
}

