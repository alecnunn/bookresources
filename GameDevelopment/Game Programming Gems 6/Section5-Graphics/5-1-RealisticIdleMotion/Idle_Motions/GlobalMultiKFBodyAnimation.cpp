#include "GlobalMultiKFBodyAnimation.h"
#include "KeyFrame.h"
#include "KeyFrameBodyAnimation.h"
#include "SinglePCAnimation.h"
#include "GlobalPCPosture.h"

#include <iostream>
using namespace std;

GlobalMultiKFBodyAnimation::GlobalMultiKFBodyAnimation(void)
{
	pcset_ = new SinglePCAnimation*[GLOBALPCSIZE];
	for (int i=0; i<GLOBALPCSIZE; i++)
		pcset_[i] = NULL;
	size_ = 0;

	keyframes_cache_ = new vector<KeyFrame*>();
	timekeys_cache_ = new vector<float>();

}

GlobalMultiKFBodyAnimation::~GlobalMultiKFBodyAnimation(void)
{
	for(int i=0;i<size_;i++)
		delete pcset_[i];
	delete [] pcset_;

	delete timekeys_cache_;
	for (i=0; i<keyframes_cache_->size(); i++)
		delete keyframes_cache_->at(i);
	delete keyframes_cache_;
}

float GlobalMultiKFBodyAnimation::getLastTimeKey() const {
	if (size_ == 0)
		return 0.0f;

	float minLength=pcset_[0]->getLastTimeKey();
	for (int i=0; i<size_; i++) {
		float lastTK = pcset_[i]->getLastTimeKey();
		if (lastTK < minLength)
			minLength = lastTK;
	}
	return minLength;
}

float GlobalMultiKFBodyAnimation::getFirstTimeKey() const {
	if (size_ == 0)
		return 0.0f;

	float minLength=pcset_[0]->getFirstTimeKey();
	for (int i=0; i<size_; i++) {
		float firstTK = pcset_[i]->getFirstTimeKey();
		if (firstTK > minLength)
			minLength = firstTK;
	}
	return minLength;
}


void GlobalMultiKFBodyAnimation::addSinglePCAnimation(SinglePCAnimation* s) {
	// check if the pcnr is already in the list
	for (int i=0; i<size_;i++) {
		if (pcset_[i]->getPCNr() == s->getPCNr())
			return;
	}
	pcset_[size_] = s;
	size_++;
	clearCache();
}

SinglePCAnimation* GlobalMultiKFBodyAnimation::getSinglePCAnimation(const int& pcnr) {
	for (int i=0; i<size_; i++) {
		if (pcset_[i]->getPCNr() == pcnr)
			return pcset_[i];
	}
	return NULL;
}

KeyFrame* GlobalMultiKFBodyAnimation::getFrame(float timekey) {
	
	// check if it is in the cache
	int c = 0;
	while (c<timekeys_cache_->size() && timekeys_cache_->at(c) != timekey) {
		c++;
	}
	if (c<timekeys_cache_->size() && timekeys_cache_->at(c) == timekey)
		return keyframes_cache_->at(c);

	
	// construct the global pc posture
	GlobalPCPosture* p = new GlobalPCPosture();

	for (int i=0; i<size_; i++) {
		//cout << "getting nr of pc " << i << endl;
		int pcnr = pcset_[i]->getPCNr();
		//cout << "getting value of pc " << endl;
		float pcval = pcset_[i]->getValue(timekey);
		//cout << "setting value in posture " << endl;
		p->setValue(pcnr,pcval);
	}

	KeyFrame* k = new KeyFrame(p);
	timekeys_cache_->push_back(timekey);
	keyframes_cache_->push_back(k);
	return k;

	
}

void GlobalMultiKFBodyAnimation::convertToKFBA(const int& fps, KeyFrameBodyAnimation* k) {
	if (k == NULL)
		return;
	k->clear();

	float step = 1000.0f / float(fps);
	float maxtime = this->getLastTimeKey();

	for (float f=0.0f; f<=maxtime; f+=step)
		k->addKeyFrame(getFrame(f),f);

	// add the last keyframe if necessary
	if (k->getLastTimeKey() < maxtime)
		k->addKeyFrame(getFrame(maxtime),maxtime);

}

void GlobalMultiKFBodyAnimation::removeKeyFramesBefore(const float& timekey) {
	for (int i=0; i<size_; i++)
		pcset_[i]->removeKeyFramesBefore(timekey);
	clearCache();
}

void GlobalMultiKFBodyAnimation::clear() {
	for (int i=0; i<size_; i++)
		pcset_[i]->clear();
	clearCache();
}

void GlobalMultiKFBodyAnimation::clearCache() {
	for (int i=keyframes_cache_->size()-1; i>=0; i--)
		delete keyframes_cache_->at(i);
	keyframes_cache_->clear();
	timekeys_cache_->clear();
}