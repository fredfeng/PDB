
#ifndef PDB_CLIENT_H
#define PDB_CLIENT_H

#include "ChrisCatalog.h"
#include "PDBCatalog.h"
#include "PDBCommunicator.h"
#include "PDBData.h"
#include "PDBQueryExecutor.h"
#include <string>
#include <vector>

using namespace std;

// This allows a program to communicate with a PDB Server
// 
class PDBClient {

public:

	// this sets up a client over the internet; on an error, wasError is set to true, and
	// the error message is in errMsg
	void connectToServer (string serverAddress, int portNumber, bool &wasError, string &errMsg, 
		size_t bufferMem, string logFile);

	// runs a query; on an error, wasError is set to true, and the error message is in errMsg
	PDBQueryExecutorPtr runQuery (PDBQueryExecutorPtr runMe, bool &wasError, string &errMsg);	

	// runs a feature list builder; on an error, wasError is set to true, and the error message is in errMsg
	void buildFeatures (PDBFeatureListBuilder &runMe, bool &wasError, string &errMsg);

	// adds data to the database; on an error, wasError is set to true, and the error message is in errMsg
	void addData (PDBData &dataToStore, bool &wasError, string &errMsg);  

	// adds a list of data to the database; on an error, wasError is set to true, and the error
	// message is in errMsg
	void addData (vector <PDBData> &dataToStore, bool &wasError, string &errMsg);

	// tells the server to shut down
	void shutDownServer (bool &wasError, string &errMsg);

	// tells the server to store data in anothr location
	void changeStorageLocation (string fName, bool &wasError, string &errMsg);

	// gets the catalog
	PDBCatalog &getCatalog ();
private:

	// used to keep track of the sequence number of the message
	int counter;

	// used to talk to the server
	PDBCommunicator myCommunicator;

	// the logger
	PDBLoggerPtr myLoggerPtr;

	// the catalog
	ChrisCatalog catalog;
};

#endif
