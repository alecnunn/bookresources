#include "KeyFrameBodyAnimation.h"
#include "joints.h"
#include "Posture.h"
#include "GlobalPCPosture.h"

#include <math.h>

#include <iostream>

using namespace std;
using namespace pcjoints;

KeyFrameBodyAnimation::KeyFrameBodyAnimation() {
	keyframes_ = NULL;
	timekeys_ = NULL;

	keyframes_cache_ = new vector<KeyFrame*>();
	timekeys_cache_ = new vector<float>();
}

KeyFrameBodyAnimation::KeyFrameBodyAnimation(KeyFrameBodyAnimation* kfba) {
	keyframes_ = NULL;
	timekeys_ = NULL;

	keyframes_cache_ = new vector<KeyFrame*>();
	timekeys_cache_ = new vector<float>();

	if (kfba->keyframes_ != NULL && kfba->timekeys_ != NULL) {
		keyframes_ = new vector<KeyFrame*>();
		timekeys_ = new vector<float>();
		for (int i=0; i<int(kfba->keyframes_->size()); i++) {
			KeyFrame* k = new KeyFrame(kfba->keyframes_->at(i));
			keyframes_->push_back(k);
			timekeys_->push_back(kfba->timekeys_->at(i));
		}
	}
}



KeyFrameBodyAnimation::~KeyFrameBodyAnimation(void)
{
	delete timekeys_;
	if (keyframes_ != NULL) {
		for (int i=0; i<keyframes_->size(); i++)
			delete keyframes_->at(i);
		delete keyframes_;
	}

	delete timekeys_cache_;
	for (int i=0; i<keyframes_cache_->size(); i++)
		delete keyframes_cache_->at(i);
	delete keyframes_cache_;

	
}


void KeyFrameBodyAnimation::sort() {

	bool changed;
	do {
		changed = false;
		vector<float>::iterator i_timekeys = timekeys_->begin();
		vector<KeyFrame*>::iterator i_keyframes = keyframes_->begin();

		while (i_timekeys < timekeys_->end()-1) {

			if (*i_timekeys > *(i_timekeys+1)) {				

				swap(*i_timekeys,*(i_timekeys+1));
				swap(*i_keyframes,*(i_keyframes+1));

				changed = true;
			}

			i_timekeys++;
			i_keyframes++;
		}
	} while (changed);

}

float KeyFrameBodyAnimation::getLastTimeKey() const {
	if (timekeys_ == NULL)
		return 0.0f;
	else
		return timekeys_->back();
}

float KeyFrameBodyAnimation::getFirstTimeKey() const {
	if (timekeys_ == NULL)
		return 0.0f;
	else
		return timekeys_->front();
}


int KeyFrameBodyAnimation::getFrameAmount() const {
	return (int)timekeys_->size();
}

KeyFrame* KeyFrameBodyAnimation::getFrame(const int& index) const {
	if (index < 0 || index >= keyframes_->size())
		return NULL;
	
	return keyframes_->at(index);
}

void KeyFrameBodyAnimation::setToBase(baseTp b) {
	if (keyframes_ == NULL)
		return;
	for (int i=0; i<keyframes_->size(); i++)
		keyframes_->at(i)->setBaseType(b);
}

std::vector<float> KeyFrameBodyAnimation::getAllTimeKeys() {
	std::vector<float> v;
	for (int i=0; i<timekeys_->size(); i++)
		v.push_back(timekeys_->at(i));
	return v;
}

void KeyFrameBodyAnimation::addKeyFrame(KeyFrame* k, float timekey) {

	if (keyframes_ == NULL)
		keyframes_ = new vector<KeyFrame*>();
	if (timekeys_ == NULL)
		timekeys_ = new vector<float>();

	// check if the time key is already in the list
	for (int i=0; i<timekeys_->size(); i++)
		if (timekeys_->at(i) == timekey)
			return;

	KeyFrame* kcopy = new KeyFrame(k);
	keyframes_->push_back(kcopy);
	timekeys_->push_back(timekey);

	this->clearCache();
	sort();
}

void KeyFrameBodyAnimation::clearCache() {
	for (int i=keyframes_cache_->size()-1; i>=0; i--)
		delete keyframes_cache_->at(i);
	keyframes_cache_->clear();
	timekeys_cache_->clear();
}

void KeyFrameBodyAnimation::removeTimeOffset() {
	if (timekeys_->empty())
		return;
	float offset=timekeys_->at(0);
	if (offset == 0.0f)
		return;
	vector<float>::iterator it = timekeys_->begin();
	while (it != timekeys_->end()) {
		*it = *it-offset;
		it++;
	}

	this->clearCache();

}

void KeyFrameBodyAnimation::retarget(std::vector<KeyFrame*>* newKFs, std::vector<int>* frameNrs) {
	int frame0=0,frame1=0;

	for (int i=0; i<frameNrs->size(); i++) {
		frame0=frame1;
		frame1=frameNrs->at(i);
		if (frame1!=0) {
			retarget_part(frame0,frame1,newKFs->at(frame1),true);
		} else {
			// we have to retarget for the first frame
			retarget_part(0,this->getFrameAmount()-1,newKFs->at(0),false);
		}
	}
	// finish retargeting by updating toward the last frame
	if (frameNrs->back() != this->getFrameAmount()-1 && frameNrs->back() != 0)
		retarget_part(frameNrs->back(),this->getFrameAmount()-1, newKFs->back(),false);
}

void KeyFrameBodyAnimation::retarget_part(int frame0, int frame1, KeyFrame* newkf, bool offsetOfFrame1, const bool& keepTrans) {
	if (frame1 <= frame0)
		throw;

	if (offsetOfFrame1) {
		// calculate offset of frame1 to original frame
		//cout << "calculating offset" << endl;
		GlobalPCPosture* original = keyframes_->at(frame1)->getGlobalPCPosture();
		float* offset = new float[GLOBALPCSIZE];
		for (int u=0; u<GLOBALPCSIZE;u++)
			offset[u] = newkf->getGlobalPCPosture()->getValue(u)-original->getValue(u);
	
		cout << "calculating new frames <offsetofframe1>" << endl;
		// from frame0 to frame1, calculate the new frames
		for (int f=frame0; f<=frame1; f++) {
			translation root_trans;
			keyframes_->at(f)->getPosture()->getTranslation(pcjoints::root,root_trans);
			GlobalPCPosture* currFrame = keyframes_->at(f)->getGlobalPCPosture();
			//cout << "changing globalpcs" << endl;
			for (int x=0; x<GLOBALPCSIZE; x++) {
				// get the old pc value
				float pcval = currFrame->getValue(x);
				// apply the offset
				pcval = pcval + (offset[x]*float(frame0-f)/float(frame0-frame1));
				currFrame->setValue(x,pcval);

			}
			//cout << "done changing globalpcs" << endl;
			// reset the translation value if required
			if (keepTrans) {
				//cout << "changing translation value" << endl;
				keyframes_->at(f)->getPosture()->setTranslation(pcjoints::root,root_trans);
			}

			// call update changes to force update
			keyframes_->at(f)->updateChanges();
		}	
	} else {
		// calculate offset of frame0 to original frame
		//cout << "calculating offset" << endl;
		GlobalPCPosture* original = keyframes_->at(frame0)->getGlobalPCPosture();
		float* offset = new float[GLOBALPCSIZE];
		for (int u=0; u<GLOBALPCSIZE;u++)
			offset[u] = newkf->getGlobalPCPosture()->getValue(u)-original->getValue(u);
		//cout << "calculating new frames <offsetofframe0>" << endl;

		// from frame0 to frame1, calculate the new frames
		for (int f=frame0; f<=frame1; f++) {
			//cout << "getting root translation" << endl;
			translation root_trans;
			keyframes_->at(f)->getPosture()->getTranslation(pcjoints::root,root_trans);

			//cout << "getting globalpcposture object" << endl;
			GlobalPCPosture* currFrame = keyframes_->at(f)->getGlobalPCPosture();
			//cout << "changing globalpcs" << endl;
			for (int x=0; x<GLOBALPCSIZE; x++) {
				// get the old pc value
				float pcval = currFrame->getValue(x);
				// apply the offset
				pcval = pcval + (offset[x]*float(frame1-f)/float(frame1-frame0));
				currFrame->setValue(x,pcval);
			}

			// reset the translation value if required
			if (keepTrans) {
				//cout << "changing translation value" << endl;
				keyframes_->at(f)->getPosture()->setTranslation(pcjoints::root,root_trans);
			}
			
			// call update changes to force update
			keyframes_->at(f)->updateChanges();
		}
	}		
	//cout << "clearing cache" << endl;
	this->clearCache();
	//cout << "done" << endl;
}

void KeyFrameBodyAnimation::applyTransOffset(const translation& t) {
	for (int i=0; i<keyframes_->size(); i++) {
		Posture* p = keyframes_->at(i)->getPosture();
		translation currTrans;
		p->getTranslation(root,currTrans);
		currTrans.x += t.x;
		currTrans.y += t.y;
		currTrans.z += t.z;
		p->setTranslation(root,currTrans);
	}
	this->clearCache();
}

void KeyFrameBodyAnimation::removeKeyFramesBefore(const float& timekey) {
	if (timekeys_ == NULL || keyframes_ == NULL)
		return;

	int x=0;
	if (timekeys_->at(0) >= timekey)
		return;
	while (timekeys_->at(x) < timekey && x < timekeys_->size())
		x++;
	// one special case x=1
	if (x == 1)
		return;

	if (x >= timekeys_->size()) {
		keyframes_->clear();
		timekeys_->clear();
	} else {
		keyframes_->erase(keyframes_->begin(),keyframes_->begin()+x-1);
		timekeys_->erase(timekeys_->begin(),timekeys_->begin()+x-1);
	}
	this->clearCache();
}

void KeyFrameBodyAnimation::removeKeyFramesAfter(const float& timekey) {
	if (timekeys_ == NULL || keyframes_ == NULL)
		return;

	if (timekey < this->getFirstTimeKey()) {
		this->clear();
		return;
	}

	while (this->getLastTimeKey() > timekey) {
		delete keyframes_->back();
		keyframes_->pop_back();
		timekeys_->pop_back();
	}

	this->clearCache();
}

void KeyFrameBodyAnimation::clear() {
	delete timekeys_;
	if (keyframes_ != NULL) {
		for (int i=0; i<keyframes_->size(); i++)
			delete keyframes_->at(i);
	}
	delete keyframes_;

	clearCache();

	keyframes_ = NULL;
	timekeys_ = NULL;
}

float KeyFrameBodyAnimation::getFrameTimeKey(const int& framenr) const {
	if (framenr < 0 || framenr >= timekeys_->size())
		return -1.0f;
	else
		return (timekeys_->at(framenr));
}