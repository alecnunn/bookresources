/* Copyright (C) Greg Alt and Kristin King, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Alt and Kristin King, 2001"
 */

// The following pseudo-code shows the main components of 
// classes AIMemoryElement and AIMemory.
 
class AIMemoryElement
{
public:
	AIMemoryElement(AIEvent);
	EventID GetID();
	int GetMagnitude();
	Time GetTimeStamp();
	bool Match(SubjectGrp, Magnitude, Verb, Object, ObjectIndividual);
	Void Update(AIEvent);
 
private:
	EventID id;
	int Magnitude;
	Time TimeStamp;
};
 
class AIMemory
{
public:
	bool Merge(AIMemory);
	void Update(AIEvent);
	void AddNewMemoryElement(AIEvent);
	void ReplaceMemoryElement(AIEvent, Index);
 
private:
	static DynamicArray<AIEvent> MasterEventList;
	DynamicArray<AIMemoryElement> PerNPCLongTermMemory;
};
 
