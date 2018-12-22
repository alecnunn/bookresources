/* Copyright (C) Jeff Evertt, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jeff Evertt, 2001"
 */
class IPerfCounterMan {
public:
// Add new counter (returns the ID, 0 is failure)
	int32		AddCounter(uint32 CounterGroup,
                             const char* szCounterName);

// Forget your counter's ID? (Zero is failure)
	int32		GetCounterID(uint32 CounterGroup,
                               const char* szCounterName);

// Delete the counter
	bool		DeleteCounter(uint32 CounterID);	

// Start and Stop a counter.
	void		StartCounter(uint32 CounterID);
	void		StopCounter(uint32 CounterID);

// Draw the Counters onto the Screen (to be called once
// per frame near the end of the scene)
	void		DrawCounters();
};
