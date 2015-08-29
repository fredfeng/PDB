
#ifndef PDB_BACKEND_H
#define PDB_BACKEND_H

#include "PDBServer.h"
#include "SimplePDB.h"
#include <string>

using namespace std;

class PDBBackEnd {

public:

	// creates a PDB server backend that communicates to the front end via a UNIX
	// domain socet and handles incoming requests
	PDBBackEnd (int numThreads, string IPCfile, int memPerThread, string logFile, string catalogFile);
};


#endif


