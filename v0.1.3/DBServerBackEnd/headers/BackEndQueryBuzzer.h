
#ifndef BACK_END_QUERY_BUZZER_H
#define BACK_END_QUERY_BUZZER_H

#include "BackEndQueryWork.h"
#include "PDBAlarm.h"
#include "PDBBuzzer.h"
#include <string>

using namespace std;

class BackEndQueryBuzzer : public PDBBuzzer {

public:

	BackEndQueryBuzzer (BackEndQueryWork &parent) : parent (parent) {}

	void handleBuzz (PDBAlarm withMe) override {
		if (withMe == PDBAlarm :: WorkAllDone) {
			parent.queryDone ();
		} else if (withMe == PDBAlarm :: QueryError) {
			parent.setError ("got an error from a query worker");
		}
	}

private:

	BackEndQueryWork &parent;
};

#endif
