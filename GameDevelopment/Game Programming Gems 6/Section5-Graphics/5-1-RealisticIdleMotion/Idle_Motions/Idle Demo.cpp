#include "PCMain.h"
#include "KeyFrameBodyAnimation.h"
#include "Posture.h"
#include "GlobalPCPosture.h"
#include "SinglePCAnimation.h"
#include "GlobalMultiKFBodyAnimation.h"

#include <iostream>
using namespace std;

void main()
{
	// First step: initialize the PC engine
	cout << "Initializing PC engine...";
	PCMain::initialize("pca_global.txt");
	cout << "done!" << endl;

	cout << "Creating KFBA object..." << endl;
	// Now we can create a key frame body animation object
	KeyFrameBodyAnimation* kfba = new KeyFrameBodyAnimation();

	cout << "Adding a key frame..." << endl;
	// create a key frame
	GlobalPCPosture* p = new GlobalPCPosture();
	// set some values
	for (int i=0; i<GLOBALPCSIZE; i++)
		p->setValue(i,0.2f);
	KeyFrame* k = new KeyFrame(p);

	// add it to the body animation
	kfba->addKeyFrame(k,0.0f);

	// to obtain the skew sym representation, we can simply
	// call the method from the KeyFrame object
	Posture* pos = k->getPosture();

	// Now that we can convert from and to PC values, the
	// single PC animation object can be used to create animations
	// for a single PC. This can be linked with a Perlin noise generator
	// to generate small posture variations.
	SinglePCAnimation* spca = new SinglePCAnimation(0);

	// here we add some frames for example
	spca->appendKeyFrame(0.0f, 0.05f);
	spca->appendKeyFrame(40.0f, 0.1f);
	spca->appendKeyFrame(80.0f, 0.05f);
	spca->appendKeyFrame(120.0f, 0.1f);

	// Now we create a complete animation from this
	GlobalMultiKFBodyAnimation* gkfba = new GlobalMultiKFBodyAnimation();
	gkfba->addSinglePCAnimation(spca);

	// And we can convert it into a normal animation
	KeyFrameBodyAnimation* conv = new KeyFrameBodyAnimation();
	gkfba->convertToKFBA(25,conv);

	// finalize the PC engine
	cout << "Finalizing PC engine...";
	PCMain::finalize();
	cout << "done!" << endl;

}

