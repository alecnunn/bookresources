#include "GlobalPCPosture.h"
#include "PCConvertor.h"


GlobalPCPosture::GlobalPCPosture(void)
{
	pcvalues_ = new float[GLOBALPCSIZE];
	for (int i=0; i<GLOBALPCSIZE; i++)
		pcvalues_[i] = 0.0f;
	changed_ = false;
}

GlobalPCPosture::GlobalPCPosture(float* pcarray) {
	pcvalues_ = new float[GLOBALPCSIZE];
	for (int i=0; i<GLOBALPCSIZE; i++)
		pcvalues_[i] = pcarray[i];
	changed_ = false;
}

GlobalPCPosture::GlobalPCPosture(GlobalPCPosture* p) {
	pcvalues_ = new float[GLOBALPCSIZE];
	for (int i=0; i<GLOBALPCSIZE; i++)
		pcvalues_[i] = p->pcvalues_[i];

	changed_ = p->changed_;
}




GlobalPCPosture::~GlobalPCPosture(void)
{
	delete [] pcvalues_;
}


float GlobalPCPosture::getValue(const int& index) {
	if (index < 0 || index >= GLOBALPCSIZE)
		return 0.0f;
	else {
		return pcvalues_[index];
	}
}

void GlobalPCPosture::setValue(const int& index, const float& value) {
	if (index < 0 || index >= GLOBALPCSIZE)
		return;
	pcvalues_[index] = value;
	changed_ = true;
}

bool GlobalPCPosture::changed() {
	return changed_;
}

void GlobalPCPosture::resetChanged() {
	changed_ = false;
}

float GlobalPCPosture::distanceFrom(GlobalPCPosture* p) {
	float dist = 0.0f;
	for (int i=0; i<GLOBALPCSIZE; i++) {

		float distComp = p->pcvalues_[i] - this->pcvalues_[i];
		dist += (distComp*distComp);
	}
	return dist;
}


