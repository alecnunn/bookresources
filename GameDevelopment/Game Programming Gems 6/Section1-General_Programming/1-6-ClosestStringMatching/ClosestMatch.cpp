// ClosestMatch.cpp : Defines the entry point for the console application.
//

#pragma warning(disable:4786)

#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <conio.h>

using namespace std;

const float CAP_MISMATCH_VAL = 0.9f;

// This is the actual string matching algorithm.  It
// returns a value from zero to one indicating an approximate 
// percentage of how closely two strings match.
float stringMatch(char const *left, char const *right)
{
    // get the length of the left, right, and longest string 
    // (to use as a basis for comparison in value calculateions)
    size_t leftSize = strlen(left);
    size_t rightSize = strlen(right);
    size_t largerSize = (leftSize > rightSize) ? 
	    leftSize : rightSize;
    char const *leftPtr = left;
    char const *rightPtr = right;
    float matchVal = 0.0f;
	// Iterate through the left string until we run out of room
    while(leftPtr != (left + leftSize) && 
	    rightPtr != (right + rightSize))
    {
        // First, check for a simple left/right match
        if(*leftPtr == *rightPtr)
        {
            // If it matches, add a proportional character's value
            // to the match total.
            matchVal += 1.0f / largerSize;
            // Advance both pointers
            ++leftPtr; 
			++rightPtr;
        }
        // If the simple match fails, 
        // try a match ignoring capitalization
        else if(::tolower(*leftPtr) == ::tolower(*rightPtr))
        {
            // We'll consider a capitalization mismatch worth 90% 
            // of a normal match
            matchVal += CAP_MISMATCH_VAL / largerSize;
            // Advance both pointers
            ++leftPtr; 
			++rightPtr;
        }
        else
        {
            char const *lpbest = left + leftSize;
            char const *rpbest = right + rightSize;
            int totalCount = 0;
            int bestCount = INT_MAX;
            int leftCount = 0;
            int rightCount = 0;
            // Here we loop through the left word in an outer loop,
            // but we also check for an early out by ensuring we
            // don't exceed our best current count
            for(char const *lp = leftPtr; (lp != (left + leftSize) 
                && ((leftCount + rightCount) < bestCount)); ++lp)
            {
                // Inner loop counting
                for(char const *rp = rightPtr; (rp != (right + 
                    rightSize) && ((leftCount + rightCount) < 
                    bestCount)); ++rp)
                {
                    // At this point, we don't care about case
                    if(::tolower(*lp) == ::tolower(*rp))
                    {
                        // This is the fitness measurement
                        totalCount = leftCount + rightCount;
                        if(totalCount < bestCount)
                        {
                            bestCount = totalCount;
                            lpbest = lp;
                            rpbest = rp;
                        }
                    }
                    ++rightCount;
                }
                ++leftCount;
                rightCount = 0;
            }
            leftPtr = lpbest;
            rightPtr = rpbest;
        }
    }
    // clamp the value in case of floating-point error
    if(matchVal > 0.99f)
        matchVal = 1.0f;
    else if(matchVal < 0.01f)
        matchVal = 0.0f;
    return matchVal;
}



// This is an example of a template function that can be used to find
// a closest match in a std map, assuming the map uses strings as
// the primary index.  Note that we must search through every string
// in the map to find the closest match, so it is a very
// slow function: O(n), which is linear time.  It's handy for 
// debugging during development, but should not be used in
// release builds.
template<class _InIt> inline
_InIt closestMatch(_InIt _First, _InIt _Last, char const *val, float limit)
{
    float maxVal = limit;
    _InIt _max = _Last;
    for (; _First != _Last; ++_First)
    {
        float newVal = stringMatch(_First->first.c_str(), val);
        if (newVal > maxVal)
        {
            maxVal = newVal;
            _max = _First;
        }
    }
    return (_max);
}

typedef std::map<std::string, int> IntMap;
IntMap imap;


// This function is a sample of an error reporting function.  It attempts
// to find the closest match within a specified tolerance.  If it can't
// find the intended ID string, it reports this to the user.
void tryMatch(string name)
{
    IntMap::iterator it;
    it = closestMatch(imap.begin(), imap.end(), name.c_str(), 0.5f);
    if(it == imap.end())
    {
        cout << "String ID :'" << name.c_str() << "' was not found within a 50% margin of error" << endl;
    }
    else
    {
        cout << "Closest match to input: '" << name.c_str() << "' is value '" 
            << it->first.c_str() << "'" << endl;
    }
}



int main(int argc, char* argv[])
{
    // Create a map of string IDs mapped to miscellaneous values.  
    // We'll test against these ID values
    imap.insert(make_pair(string("RandomValue"), 1));
    imap.insert(make_pair(string("AnotherValue"), 2));
    imap.insert(make_pair(string("NothingLikeIt"), 3));
    imap.insert(make_pair(string("BigTimeString"), 4));
    imap.insert(make_pair(string("NoName"), 5));
    imap.insert(make_pair(string("WhatsInAName"), 6));
    imap.insert(make_pair(string("SimpleIdea"), 7));
    imap.insert(make_pair(string("PrefixTest"), 8));
    imap.insert(make_pair(string("PrefixWaffle"), 9));
    imap.insert(make_pair(string("PrefixSyrup"), 10));
    imap.insert(make_pair(string("JackJillBattle"), 11));
    imap.insert(make_pair(string("SweetBattle"), 12));
    imap.insert(make_pair(string("SimpleBattle"), 13));

    // Now we find out if the matching algorithm can find a 
    // close-enough match (within 50% tolerance) among all
	// the string IDs.  Note that this type of function would
	// only be called AFTER a normal string-based lookup
	// had already failed.
    tryMatch("what's in a game");
    tryMatch("nother valu");
    tryMatch("JackJillFight");
    tryMatch("PreSyrupp");
    tryMatch("sweets_batle");
    tryMatch("completely bogus data");

	cout << endl << "Press any key to continue...";
	getch();

	return 0;
}

