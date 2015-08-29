
#ifndef NOTHING_WORK_H
#define NOTHING_WORK_H

#include "PDBBuzzer.h"
#include "PDBCommWork.h"

// do no work
class NothingWork : public PDBWork {

public:

	NothingWork () {}

	void execute (PDBBuzzerPtr callerBuzzer) override {callerBuzzer = nullptr; /* so we don't get a compiler error */}

};

#endif

