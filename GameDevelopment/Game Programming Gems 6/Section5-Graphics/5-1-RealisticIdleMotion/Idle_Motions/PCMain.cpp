#include "PCMain.h"
#include "PCConvertor.h"

PCConvertor* PCMain::convertor = NULL;

void PCMain::initialize(	const std::string& globalPCFile) {
	try {
		convertor = new PCConvertor(globalPCFile);
	} catch(...) {
		convertor = NULL;
	}
}

void PCMain::finalize() {
	delete convertor;
}
