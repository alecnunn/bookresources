#include "PCConvertor.h"
#include "GlobalPCPosture.h"

#include <fstream>
#include <math.h>

using namespace std;
using namespace pcjoints;

PCConvertor::PCConvertor(	const std::string& globalPCFile)
{
	// initialize the joint maps
	pcjoints::initJointMap_global(globalJointMap_);

	// read the global PC matrix
	globalpcmatrix_ = new float*[GLOBALPCSIZE];
	for (int i=0; i<GLOBALPCSIZE; i++)
		globalpcmatrix_[i] = new float[GLOBALPCSIZE];
	this->readSquareMatrix(globalPCFile,globalpcmatrix_,GLOBALPCSIZE);
}

void PCConvertor::readSquareMatrix(const std::string& filename, float** matrix, int size) {
	ifstream input;
	input.open(filename.data());
	
	char c = ' ';
	
	// fill in the values
	for (int row=0; row<size; row++) {
		for (int column=0; column<size; column++) {
			// remove garbage
			while ((c < 48 || c > 57) && c != '-')
				c = input.get();

			string s="";
			while (c != ' ' && c != '\n' ) {
				s += c;
				c = input.get();
			}
			if (s != "") {
				matrix[row][column] = (float)strtod(s.data(),'\0');
			}
		}
	}
	input.close();
}


PCConvertor::~PCConvertor(void)
{
	clearMemory();

}

void PCConvertor::clearMemory() {
	for (int i=0; i<GLOBALPCSIZE; i++)
		delete [] globalpcmatrix_[i];

	delete [] globalpcmatrix_;
	globalpcmatrix_ = NULL;

}

bool PCConvertor::globalPCMatrixIsNull() {
	return (globalpcmatrix_ == NULL);
}


void PCConvertor::toPosture(GlobalPCPosture* g, Posture* p) {
	if (p == NULL || g == NULL)
		return;

	if (globalpcmatrix_ == NULL)
		return;

	float* multRes = new float[GLOBALPCSIZE];

	for (int row=0; row<GLOBALPCSIZE; row++) {

		multRes[row] = 0.0f;
		for (int column=0; column<GLOBALPCSIZE; column++)
			multRes[row] += globalpcmatrix_[row][column]*g->getValue(column);
	}

	// set the root translation value in the posture
	translation root_trans = {multRes[0], multRes[1], multRes[2]};
	p->setTranslation(root, root_trans);

	// set the rotation values

	for (int joint=0; joint<GLOBALPCJOINTS; joint++) {

		// construct the skew-symmetric matrix
		int location = joint*3 + 3;
		skewSymTp rot;

		rot.a = multRes[location];
		rot.b = multRes[location+1];
		rot.c = multRes[location+2];

		// put in posture object
		p->setRotation(pcjoints::getJoint(globalJointMap_,joint),rot);

	}

	// clean up memory
	delete [] multRes;
}

void PCConvertor::toGlobalPC(Posture* p, GlobalPCPosture* g) {

	if (globalpcmatrix_ == NULL)
		return;

	if (p == NULL || g == NULL)
		return;

	// the vector to be multiplied with the PC matrix
	float* quatTransVector = new float[GLOBALPCSIZE];
	
	// get the root translation value
	translation root_trans;
	p->getTranslation(root, root_trans);
	quatTransVector[0] = root_trans.x;
	quatTransVector[1] = root_trans.y;
	quatTransVector[2] = root_trans.z;

	// now put all the joints
	for (int joint = 0; joint < GLOBALPCJOINTS; joint++) {

		skewSymTp rot;
		p->getRotation(pcjoints::getJoint(globalJointMap_,joint),rot);

		int location = joint*3 + 3;
		quatTransVector[location]   = rot.a;
		quatTransVector[location+1] = rot.b;
		quatTransVector[location+2] = rot.c;

	}

	// calculate the product of transposed pcmatrix with quatTransVector
	for (int row=0; row<GLOBALPCSIZE; row++) {

		float currPC = 0.0f;
		for (int column=0; column<GLOBALPCSIZE; column++)
			 currPC += globalpcmatrix_[column][row]*quatTransVector[column];
		g->setValue(row,currPC);
	}


	// clean up memory
	delete [] quatTransVector;

}