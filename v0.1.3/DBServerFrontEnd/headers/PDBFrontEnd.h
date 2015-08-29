
#ifndef PDB_FRONTEND_H
#define PDB_FRONTEND_H

#include "PDBServer.h"
#include <string>

using namespace std;

class PDBFrontEnd {

public:

	// creates a PDB server frontend that listens to the specified port to handle
	// requests coming from over the network, and communicates with the PDB backend
	// using a UNIX domain socket that is bound to the file name "IPCfile".  
	PDBFrontEnd (int port, int numConnections, string IPCfile, int RAMPerConnection, string logFile);

private:

	PDBServerPtr externalServer;
};


#endif


