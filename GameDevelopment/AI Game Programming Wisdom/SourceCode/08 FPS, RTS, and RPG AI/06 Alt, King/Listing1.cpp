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
// classes EventID and AIEvent.
 
class EventID
{
public:
	EventID(SubjectGrp, Verb, ObjectGrp, ObjectIndividual);
	Group GetSubjectGroup();
	Verb GetVerb();
	Group GetObjectGroup();
	UniqueID GetObjectIndividual();

private:
	int64 id;
};
 
class AIEvent
{
public:
AIEvent(SubjectGrp, Verb, ObjectGrp, ObjectIndividual, TimeStamp,
Lifetime, EventTemplate);
	EventID GetID();
	Point3D GetPosition();
	Time GetTimeStamp();
	int GetMagnitude();
	TemplateID GetEventTemplate();
	int IncrementReferenceCount();
	int DecrementReferenceCount();
 
private:
	EventID id;
	Point3D Position;
	Time TimeStamp;
	int Magnitude;
	Template EventTemplate;
	int ReferenceCount;
};
 
