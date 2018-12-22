#include "Posture.h"
#include "Interpolator.h"
#include "PCConvertor.h"

#include <iostream>

using namespace std;
using namespace pcjoints;

Posture::Posture(void)
{
	this->setNeutral();

	changed_ = false;

}

Posture::Posture(Posture* p) {
	this->setNeutral();

	// make the iterators
	std::map<pcjoints::jointTp,translation>::iterator transit = p->transMap_.begin();
	std::map<pcjoints::jointTp,skewSymTp>::iterator rotit = p->rotMap_.begin();

	while (transit!= p->transMap_.end()) {	
		this->setTranslation(transit->first,transit->second);
		transit++;
	}
	while (rotit!= p->rotMap_.end()) {	
		this->setRotation(rotit->first,rotit->second);
		rotit++;
	}

	this->changed_ = p->changed_;
}


Posture::~Posture(void)
{
}

void Posture::getTranslation(const pcjoints::jointTp& joint, translation& trans) const {
	map<jointTp,translation>::const_iterator it = transMap_.find(joint);
	if (it != transMap_.end())
		trans = it->second;
	else {
		trans.x = 0.0f;
		trans.y = 0.0f;
		trans.z = 0.0f;
	}

}

void Posture::setTranslation(const pcjoints::jointTp& joint, translation& trans) {
	transMap_[joint] = trans;
	changed_ = true;
}

void Posture::getActiveTranslations(std::vector<pcjoints::jointTp>& v) {

	map<jointTp,translation>::iterator it = transMap_.begin();
	while (it != transMap_.end()) {
		v.push_back(it->first);
		it++;
	}
}
void Posture::getActiveRotations(std::vector<pcjoints::jointTp>& v) {

	map<jointTp,skewSymTp>::iterator it = rotMap_.begin();
	while (it != rotMap_.end()) {
		v.push_back(it->first);
		it++;
	}

}

bool Posture::changed() {
	return changed_;
}

void Posture::resetChanged() {
	changed_ = false;
}

void Posture::setNeutral() {

	transMap_.clear();
	rotMap_.clear();
	changed_ = true;
}

translation	Posture::interpolateTrans(const translation& trans1, const translation& trans2,
									  const float& location) {
	  translation t;
	  t.x = Interpolator::twoPointInterpol(0,trans1.x,1,trans2.x,location,linear);
	  t.y = Interpolator::twoPointInterpol(0,trans1.y,1,trans2.y,location,linear);
	  t.z = Interpolator::twoPointInterpol(0,trans1.z,1,trans2.z,location,linear);
	  return t;
}


void Posture::getRotation(const pcjoints::jointTp& joint, skewSymTp& r) const {

	map<jointTp,skewSymTp>::const_iterator it = rotMap_.find(joint);
	if (it == rotMap_.end()) {
		r.a = 0.0f;
		r.b = 0.0f;
		r.c = 0.0f;
	} else {
		skewSymTp rot = it->second;
	}

}

void Posture::setRotation(const pcjoints::jointTp& joint, skewSymTp& r) {
	
	rotMap_[joint] = r;
	changed_ = true;
}