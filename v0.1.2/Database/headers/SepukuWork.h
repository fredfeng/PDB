
#ifndef SEPUKU_H
#define SEPUKU_H

#include "PDBWork.h"
#include <pthread.h>

// this is a class, that when executed, causes the thread to kill itself
class SepukuWork : public PDBWork {

public:

	void execute () override {
		done ();
		pthread_exit (NULL);
	}
};

#endif

