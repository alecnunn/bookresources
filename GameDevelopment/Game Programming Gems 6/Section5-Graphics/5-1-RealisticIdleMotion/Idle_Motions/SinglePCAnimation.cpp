#include "SinglePCAnimation.h"
using namespace std;

SinglePCAnimation::SinglePCAnimation(const int& pcnr)
{
	pcnr_ = pcnr;
	pcvalues_ = new vector<float>();
	timekeys_ = new vector<float>();
}

SinglePCAnimation::~SinglePCAnimation(void)
{
	delete pcvalues_;
	delete timekeys_;
}

float SinglePCAnimation::getValue(const float& timekey, const interpolTp& i) {
	if (timekey < 0 || timekey > this->getLastTimeKey())
		return 0.0f;

	// find out between which two keyframes the timekey is
	int index=0;
	while (timekeys_->at(index) < timekey)
		index++;

	if (timekeys_->at(index) == timekey) {
		return pcvalues_->at(index);
	}

	// get begin and end points
	float x1 = timekeys_->at(index-1);
	float y1 = pcvalues_->at(index-1);
	float x2 = timekeys_->at(index);
	float y2 = pcvalues_->at(index);

	// interpolate and return the value
	return Interpolator::twoPointInterpol(x1,y1,x2,y2,timekey,i);

}

void SinglePCAnimation::appendKeyFrame(const float& timekey, const float& value) {
	if (timekey <= this->getLastTimeKey() && !timekeys_->empty())
		return;
	if (timekey < 0)
		return;
	timekeys_->push_back(timekey);
	pcvalues_->push_back(value);

}

float SinglePCAnimation::getLastTimeKey() const {
	if (timekeys_->empty())
		return 0.0f;
	else
		return timekeys_->back();
}

float SinglePCAnimation::getFirstTimeKey() const {
	if (timekeys_->empty())
		return 0.0f;
	else
		return timekeys_->front();
}

int SinglePCAnimation::getPCNr() {
	return pcnr_;
}

int SinglePCAnimation::getFrameAmount() {
	return timekeys_->size();
}

void SinglePCAnimation::removeKeyFramesBefore(const float& timekey) {
	int x=0;
	if (timekeys_->at(0) >= timekey)
		return;
	while (timekeys_->at(x) < timekey && x < timekeys_->size())
		x++;
	// one special case x=1
	if (x == 1)
		return;

	if (x >= timekeys_->size()) {
		pcvalues_->clear();
		timekeys_->clear();
	} else {
		pcvalues_->erase(pcvalues_->begin(),pcvalues_->begin()+x-1);
		timekeys_->erase(timekeys_->begin(),timekeys_->begin()+x-1);
	}
}

void SinglePCAnimation::clear() {
	pcvalues_->clear();
	timekeys_->clear();
}