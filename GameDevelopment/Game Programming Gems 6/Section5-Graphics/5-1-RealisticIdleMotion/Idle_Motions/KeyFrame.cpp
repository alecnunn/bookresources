#include "KeyFrame.h"
#include "Posture.h"
#include "PCConvertor.h"
#include "PCMain.h"
#include "GlobalPCPosture.h"

#include <math.h>

#include <iostream>

using namespace std;
using namespace pcjoints;

KeyFrame::KeyFrame() {
	globalPCs_ = NULL;
	posture_ = NULL;
	base_ = unknown;
}

KeyFrame::KeyFrame(Posture* p) {
	posture_ = p;
	globalPCs_ = NULL;
	base_ = skew;
}

KeyFrame::KeyFrame(GlobalPCPosture* p) {
	globalPCs_ = p;
	posture_ = NULL;
	base_ = globalpc;
}


KeyFrame::KeyFrame(KeyFrame* k) {

	this->copy(k);
}

void KeyFrame::copy(KeyFrame* k) {
	globalPCs_ = NULL;
	posture_ = NULL;
	base_ = k->base_;

	if (k->globalPCs_ != NULL)
		globalPCs_ = new GlobalPCPosture(k->globalPCs_);
	if (k->posture_ != NULL)
		posture_ = new Posture(k->posture_);
}



KeyFrame::~KeyFrame() {
	delete globalPCs_;
	delete posture_;
}


GlobalPCPosture* KeyFrame::getGlobalPCPosture() {
	updateChanges();

	if (globalPCs_ == NULL) {
		if (posture_ != NULL) {
			globalPCs_ = new GlobalPCPosture();
			PCMain::convertor->toGlobalPC(posture_, globalPCs_);
		} else
			return NULL;
	}
	return globalPCs_;
}

Posture* KeyFrame::getPosture() {
	updateChanges();

	if (posture_ == NULL) {
		if (globalPCs_ != NULL) {
			posture_ = new Posture();
			PCMain::convertor->toPosture(globalPCs_, posture_);
		} else {
			return NULL;
		}
	}
	return posture_;
}

void KeyFrame::updateChanges() {

	int changed=0;
	if (globalPCs_ != NULL && globalPCs_->changed())
		changed++;
	if (posture_ != NULL && posture_->changed())
		changed++;
	if (changed == 0)
		return;

	if (changed > 1) {
		cout << "ERROR: more than one representation has changed!" << endl;
		throw;
	}

	if (globalPCs_ != NULL && globalPCs_->changed()) {
		delete posture_;
		posture_ = NULL;
		globalPCs_->resetChanged();
	} else if (posture_ != NULL && posture_->changed()) {
		delete globalPCs_;
		globalPCs_ = NULL;
		posture_->resetChanged();
	}
}


void KeyFrame::setBaseType(baseTp b) {
	base_ = b;
}
baseTp KeyFrame::getBaseType() {
	return base_;
}


void KeyFrame::neutral() {
	delete globalPCs_;
	delete posture_;
	posture_ = NULL;
	globalPCs_ = NULL;
	globalPCs_ = new GlobalPCPosture();
}



