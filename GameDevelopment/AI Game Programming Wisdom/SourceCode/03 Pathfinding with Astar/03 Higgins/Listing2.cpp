/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */

// An example of the path manager's update follows:

// Note: Using Revs as short for Revolutions
// to conserve code space here
// While we have paths to process and haven't
// exceeded our pathfinding revolutions-per-tick cap...
while(this->mPathsQueue.size() &&
	theRevsCompleted < kMaxRevsPerGameTick)
{
	// Get the first pathfinder
	// impl note: mPathsQueue is an STL deque
	thePath = this->mPathsQueue.front();

	// Run, Run, Run!
	thePath->RunAStar();

	// How many revolutions so far?
	theRevsCompleted += thePath->GetRevsThisRun();

	// If we aren't still working, then we are done!
	if (!thePath->GetIsStillWorking())
	{
		// Post processing
		this->ProcessFinishedPath(thePath);
	}
	else
	{
		// Is it less than the max revolutions allowed?
		if(thePath->GetRevsPerRun() < kMaxRevsPerRun)
		{
			// Set its revolution cap X more
			// each time so the more often it moves
			// through the queue, the more it
			// processes each time.
			theRevsPerRun = thePath->GetRevsPerRun();
			thePath->SetRevsPerRun(theRevsPerRun + 1);
		}

		// Take it off the front of the queue
		// and put it on the rear of the queue.
		this->mPathsQueue.push_back(thePath);
	}

	// Remove it from the front.
	this->mPathsQueue.pop_front();
}
