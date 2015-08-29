
#ifndef EMPTY_PDB_BUZZER_H
#define EMPTY_PDB_BUZZER_H

#include "PDBAlarm.h"
#include "PDBBuzzer.h"
#include <string>

using namespace std;

// this takes no special action on any buzz, so it can be used to sleep until any alarm is sounded
class EmptyPDBBuzzer : public PDBBuzzer {

public:

        EmptyPDBBuzzer () {}

        void handleBuzz (PDBAlarm) override {}
};

#endif
